#include <PrecompiledHeader.h>
#include "Defines.h"
#include "Regions/MapManager.h"
#include "Micro/Units/ProtossSpecial/HighTemplarUnit.h"
#include "Intelligence/Intelligence.h"

#define __MESH_SIZE__ 16 // 16 // 24 // 32 // 48
#define __STORM_SIZE__ 96
#define __MAX_PATHFINDING_WORKS__ 10

using namespace BWAPI;

std::map<BWAPI::Unit*, BWAPI::Position> HighTemplarUnit::stormableUnits;

int hash(BWAPI::Position p)
{
	/// Max size for a map is 512x512 build tiles => 512*32 = 16384 = 2^14 pixels
	/// Unwalkable regions will map to 0
	int tmp = p.x() + 1; // + 1 to give room for choke dependant regions (after shifting)
	tmp = (tmp << 16) | p.y();
	return tmp;
}

int hash(BWTA::Region* r)
{
	return hash(r->getPolygon().getCenter());
}

int hash(BWTA::BaseLocation* bl)
{
	return hash(bl->getPosition());
}
void drawBTPath(const std::vector<TilePosition>& btpath)
{
    if (btpath.empty())
        return;
    for (std::vector<TilePosition>::const_iterator it = btpath.begin(); 
        it != btpath.end(); ++it)
    {
        Broodwar->drawBox(CoordinateType::Map, it->x()*32 + 2, it->y()*32 + 2, 
            it->x()*32 + 30, it->y()*32 + 30, Colors::Yellow);
    }
}

MapManager::MapManager()
: _pix_width(Broodwar->mapWidth() * 32)
, _pix_height(Broodwar->mapHeight() * 32)
, _width(Broodwar->mapWidth() * 4)
, _height(Broodwar->mapHeight() * 4)
, _stormPosMutex(CreateMutex( 
				 NULL,                  // default security attributes
				 FALSE,                 // initially not owned
				 NULL))                  // unnamed mutex
, _signalLaunchStormUpdate(CreateEvent( 
						   NULL, // default sec
						   FALSE, // manual reset?
						   FALSE, // initial state
						   TEXT("LaunchStormPosUpdate")))
, _stormThread(NULL)
, _pathfindingMutex(CreateMutex( 
				 NULL,                  // default security attributes
				 FALSE,                 // initially not owned
				 NULL))                  // unnamed mutex
, _signalLaunchPathfinding(CreateEvent(
						   NULL, // default sec
						   FALSE, // manual reset?
						   FALSE, // initial state
						   TEXT("LaunchPathfinding")))
, _pathfindingThread(NULL)
, _lastStormUpdateFrame(0)
, _eUnitsFilter(& EUnitsFilter::Instance())
, _currentPathfindWork(NULL, NULL, TilePositions::None, TilePositions::None, -1)
, _currentPathfindWorkAborded(false)
{
#ifdef __INTELLIGENCE_DEBUG__
    if (_stormPosMutex == NULL) 
    {
        Broodwar->printf("CreateMutex error: %d\n", GetLastError());
        return;
    }
    if (_signalLaunchStormUpdate == NULL) 
    {
        Broodwar->printf("CreateEvent error: %d\n", GetLastError());
        return;
    }
#endif
    walkability = new bool[_width * _height];             // Walk Tiles resolution
    buildings_wt = new bool[_width * _height];
#ifdef __BUILDINGS_WT_STRICT__
    buildings_wt_strict = new bool[_width * _height];
#endif
    _lowResWalkability = new bool[Broodwar->mapWidth() * Broodwar->mapHeight()]; // Build Tiles resolution
    buildings = new bool[Broodwar->mapWidth() * Broodwar->mapHeight()];         // [_width * _height / 16];
    _buildingsBuf = new bool[Broodwar->mapWidth() * Broodwar->mapHeight()];
    groundDamages = new int[Broodwar->mapWidth() * Broodwar->mapHeight()];
    _groundDamagesBuf = new int[Broodwar->mapWidth() * Broodwar->mapHeight()];
    airDamages = new int[Broodwar->mapWidth() * Broodwar->mapHeight()];
    _airDamagesBuf = new int[Broodwar->mapWidth() * Broodwar->mapHeight()];
    groundDamagesGrad = new Vec[Broodwar->mapWidth() * Broodwar->mapHeight()];
    airDamagesGrad = new Vec[Broodwar->mapWidth() * Broodwar->mapHeight()];

	for each (BWTA::Region* r in BWTA::getRegions())
		regionToHash.insert(std::make_pair(r, hash(r)));
	for each (BWTA::BaseLocation* bl in BWTA::getBaseLocations())
		baseLocationToHash.insert(std::make_pair(bl, hash(bl)));

	char buf[500];
	sprintf_s(buf, "bwapi-data/AI/terrain/%s.pfd", BWAPI::Broodwar->mapHash().c_str());
	if (fileExists(buf))
	{	
		std::ifstream ifs(buf, std::ios::binary);
		boost::archive::binary_iarchive ia(ifs);
		ia >> _pfMaps;
	}
	else
	{
		/// Fill regionsPFCenters (regions pathfinding aware centers, 
		/// min of the sum of the distance to chokes on paths between/to chokes)
		const std::set<BWTA::Region*> allRegions = BWTA::getRegions();
		for (std::set<BWTA::Region*>::const_iterator it = allRegions.begin();
			it != allRegions.end(); ++it)
		{
			std::list<Position> chokesCenters;
			for (std::set<BWTA::Chokepoint*>::const_iterator it2 = (*it)->getChokepoints().begin();
				it2 != (*it)->getChokepoints().end(); ++it2)
				chokesCenters.push_back((*it2)->getCenter());
			if (chokesCenters.empty())
				_pfMaps.regionsPFCenters.insert(std::make_pair(hash(*it), std::make_pair((*it)->getCenter().x(), (*it)->getCenter().y())));
			else
			{
				std::list<TilePosition> validTilePositions;
				for (std::list<Position>::const_iterator c1 = chokesCenters.begin();
					c1 != chokesCenters.end(); ++c1)
				{
					for (std::list<Position>::const_iterator c2 = chokesCenters.begin();
						c2 != chokesCenters.end(); ++c2)
					{
						if (*c1 != *c2)
						{
							std::vector<TilePosition> buffer = BWTA::getShortestPath(TilePosition(*c1), TilePosition(*c2));
							for (std::vector<TilePosition>::const_iterator vp = buffer.begin();
								vp != buffer.end(); ++vp)
								validTilePositions.push_back(*vp);
						}
					}
				}
				double minDist = DBL_MAX;
				TilePosition centerCandidate = TilePosition((*it)->getCenter());
				for (std::list<TilePosition>::const_iterator vp = validTilePositions.begin();
					vp != validTilePositions.end(); ++vp)
				{
					double tmp = 0.0;
					for (std::list<Position>::const_iterator c = chokesCenters.begin();
						c != chokesCenters.end(); ++c)
					{
						tmp += BWTA::getGroundDistance(TilePosition(*c), *vp);
					}
					if (tmp < minDist)
					{
						minDist = tmp;
						centerCandidate = *vp;
					}
				}
				Position tmp(centerCandidate);
				_pfMaps.regionsPFCenters.insert(std::make_pair(hash(*it), std::make_pair(tmp.x(), tmp.y())));
			}
		}

		/// Fill distRegions with the mean distance between each Regions
		/// -1 if the 2 Regions are not mutualy/inter accessible by ground
		for (std::set<BWTA::Region*>::const_iterator it = allRegions.begin();
			it != allRegions.end(); ++it)
		{
			_pfMaps.distRegions.insert(std::make_pair(hash(*it),
				std::map<int, double>()));
			for (std::set<BWTA::Region*>::const_iterator it2 = allRegions.begin();
				it2 != allRegions.end(); ++it2)
				_pfMaps.distRegions[hash(*it)].insert(std::make_pair(hash(*it2), 
					BWTA::getGroundDistance(TilePosition(regionsPFCenters(*it)),
					TilePosition(regionsPFCenters(*it2)))));
		}

		/// Fill distBaseToBase
		for each (BWTA::BaseLocation* b1 in BWTA::getBaseLocations())
		{
			_pfMaps.distBaseToBase.insert(std::make_pair(hash(b1),
				std::map<int, double>()));
			for each (BWTA::BaseLocation* b2 in BWTA::getBaseLocations())
			{
				_pfMaps.distBaseToBase[hash(b1)].insert(std::make_pair(hash(b2),
					BWTA::getGroundDistance(b1->getTilePosition(), b2->getTilePosition())));
			}
		}

		std::ofstream ofs(buf, std::ios::binary);
		{
			boost::archive::binary_oarchive oa(ofs);
			oa << _pfMaps;
		}
	}

    /// initialization of all the walkability tables
    for (int x = 0; x < _width; ++x) 
        for (int y = 0; y < _height; ++y) 
        {
            walkability[x + y*_width] = Broodwar->isWalkable(x, y);
            buildings_wt[x + y*_width] = false;
#ifdef __BUILDINGS_WT_STRICT__
            buildings_wt_strict[x + y*_width] = false;
#endif
        }
    for (int x = 0; x < _width/4; ++x) 
    {
        for (int y = 0; y < _height/4; ++y) 
        {
            int xx = 4*x;
            int yy = 4*y;
            bool walkable = true;
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j) 
                {
                    if (!walkability[xx+i + (yy+j)*_width])
                        walkable = false;
                }
            }
            _lowResWalkability[x + y*_width/4] = walkable;
            buildings[x + y*_width/4] = false; // initialized with manual call to onUnitCreate() in onStart()
            groundDamages[x + y*_width/4] = 0;
            groundDamagesGrad[x + y*_width/4] = Vec(0, 0);
            airDamages[x + y*_width/4] = 0;
            airDamagesGrad[x + y*_width/4] = Vec(0, 0);
            _buildingsBuf[x + y*_width/4] = false; // because we use pathfinding in the init, otherwise no need
        }
    }

	/// Precomputed paths (TODO put in update() and use the PF thread)
	BWTA::BaseLocation* home = BWTA::getStartLocation(Broodwar->self());
	for each (BWTA::BaseLocation* b in BWTA::getStartLocations())
	{
		if (b == home)
			continue;
		std::vector<BWAPI::TilePosition> tmpPath;
		buildingsAwarePathFind(tmpPath, home->getTilePosition(), b->getTilePosition());
		_pathsFromHomeToSL.insert(std::make_pair<BWTA::BaseLocation*, std::vector<TilePosition> >(b, tmpPath));
	}
}

MapManager::~MapManager()
{
#ifdef __INTELLIGENCE_DEBUG__
    Broodwar->printf("MapManager destructor started");
#endif
    delete [] walkability;
    delete [] buildings_wt;
#ifdef __BUILDINGS_WT_STRICT__
    delete [] buildings_wt_strict;
#endif
    delete [] buildings;
    delete [] groundDamages;
    delete [] airDamages;
    delete [] groundDamagesGrad;
    delete [] airDamagesGrad;
	TerminateThread(_stormThread, 0);
	TerminateThread(_pathfindingThread, 0);
    CloseHandle(_stormPosMutex);
    CloseHandle(_signalLaunchStormUpdate);
    CloseHandle(_stormThread);
	CloseHandle(_pathfindingMutex);
	CloseHandle(_pathfindingThread);
    delete [] _lowResWalkability;
	delete [] _buildingsBuf;
    delete [] _groundDamagesBuf;
	delete [] _airDamagesBuf;
#ifdef __INTELLIGENCE_DEBUG__
    Broodwar->printf("MapManager destructor finished");
#endif
}

void MapManager::modifyBuildings(Unit* u, bool b)
{
    // TODO Optimize (3 loops are not necessary)
    if (!u->getType().isBuilding() 
        || (u->isLifted() && b)) // lifted building won't be added (b==true)
        return;
    TilePosition tpBd = u->getTilePosition(); // top left corner of the building
    for (int x = tpBd.x(); x < tpBd.x() + u->getType().tileWidth(); ++x)
        for (int y = tpBd.y(); y < tpBd.y() + u->getType().tileHeight(); ++y)
        {
            buildings[x + y*Broodwar->mapWidth()] = b;
        }
    for (int x = tpBd.x()*4 - 1; x < tpBd.x()*4 + u->getType().tileWidth()*4 + 1; ++x)
        for (int y = tpBd.y()*4 - 1; y < tpBd.y()*4 + u->getType().tileHeight()*4 + 1; ++y)
            if (x >= 0 && x < _width && y >= 0 && y < _height)
                buildings_wt[x + y*_width] = b;
#ifdef __BUILDINGS_WT_STRICT__
    for (int x = (u->getPosition().x() - u->getType().dimensionLeft() - 5) / 8; 
        x <= (u->getPosition().x() + u->getType().dimensionRight() + 5) / 8; ++x) // x += 8
    {
        for (int y = (u->getPosition().y() - u->getType().dimensionUp() - 5) / 8;
            y <= (u->getPosition().y() + u->getType().dimensionDown() + 5) / 8; ++y) // y += 8
        {
            buildings_wt_strict[x + y*_width] = b;
            //buildings_wt[x + y*_width] = b;
            //if (y > 0) buildings_wt[x + (y-1)*_width] = b;
        }
    }
#endif
}

void MapManager::addBuilding(Unit* u)
{
    //Broodwar->printf("%x %s \n", u, u->getType().getName().c_str());
    modifyBuildings(u, true);
}

void MapManager::addAlliedUnit(Unit* u)
{
    if (!(u->getType().isBuilding()) && !_ourUnits.count(u))
        _ourUnits.insert(std::make_pair<Unit*, Position>(u, u->getPosition()));
}

void MapManager::removeBuilding(Unit* u)
{
    modifyBuildings(u, false);
}

void MapManager::removeAlliedUnit(Unit* u)
{
    if (!(u->getType().isBuilding()))
        _ourUnits.erase(u);
}

void MapManager::modifyDamages(int* tab, Position p, int minRadius, int maxRadius, int damages)
{
    // TODO optimize
    int tmpMaxRadius = maxRadius; /// + 32; // TOCHANGE 32
    //Broodwar->printf("modify minRadius: %d, maxRadius %d, Position(%d, %d)", minRadius, maxRadius, p.x(), p.y());
    int lowerX = (p.x() - tmpMaxRadius) > 0 ? p.x() - tmpMaxRadius : 0;
    int higherX = (p.x() + tmpMaxRadius) < _pix_width ? p.x() + tmpMaxRadius : _pix_width;
    int lowerY = (p.y() - tmpMaxRadius) > 0 ? p.y() - tmpMaxRadius : 0;
    int higherY = (p.y() + tmpMaxRadius) < _pix_height ? p.y() + tmpMaxRadius : _pix_height;
    assert(higherX > lowerX);
    assert(higherY > lowerY);
    for (int x = lowerX; x <= higherX; x += 32)
        for (int y = lowerY; y <= higherY; y += 32)
        {
            double dist = p.getDistance(Position(x, y));
            if (dist <= tmpMaxRadius && dist > minRadius)
            {
                tab[x/32 + y/32*Broodwar->mapWidth()] += damages;
            }                
        }
}

void MapManager::updateDamagesGrad(Vec* grad, int* tab, Position p, int minRadius, int maxRadius)
{
    int tmpMaxRadius = maxRadius + 32 + 46; // 32 b/c it is a gradient, 46 for enemy unit movement
    int tmpMinRadius = max(0, minRadius - 32);
    int lowerX = (p.x() - tmpMaxRadius) > 0 ? p.x() - tmpMaxRadius : 0;
    int higherX = (p.x() + tmpMaxRadius) < _pix_width ? p.x() + tmpMaxRadius : _pix_width;
    int lowerY = (p.y() - tmpMaxRadius) > 0 ? p.y() - tmpMaxRadius : 0;
    int higherY = (p.y() + tmpMaxRadius) < _pix_height ? p.y() + tmpMaxRadius : _pix_height;
    assert(higherX > lowerX);
    assert(higherY > lowerY);
    for (int x = lowerX; x <= higherX; x += 32)
        for (int y = lowerY; y <= higherY; y += 32)
        {
            double dist = p.getDistance(Position(x, y));
            if (dist <= tmpMaxRadius && dist > tmpMinRadius)
            {
                int xx = x/32;
                int yy = y/32;
                grad[xx + yy*Broodwar->mapWidth()] = Vec(0, 0);
				if (!tab[xx + yy*Broodwar->mapWidth()])
					continue;
                for (int tmpx = xx - 1; tmpx <= xx + 1; ++tmpx)
                    for (int tmpy = yy - 1; tmpy <= yy + 1; ++tmpy)
                    {
                        int deltax = tmpx - xx;
                        int deltay = tmpy - yy;
                        if (deltax == 0 && deltay == 0)
                            continue;
                        if (tmpx < 0 || tmpy < 0)
                            continue;
                        if (tmpx >= Broodwar->mapWidth() || tmpy >= Broodwar->mapHeight())
                            continue;
                        grad[xx + yy*Broodwar->mapWidth()] += 
                            Vec(deltax, deltay)*tab[tmpx + (tmpy)*Broodwar->mapWidth()];
                    }
            }                
        }
}

void MapManager::removeDmg(UnitType ut, Position p)
{
    if (ut.groundWeapon() != BWAPI::WeaponTypes::None)
    {
        int addRange = additionalRangeGround(ut);
        modifyDamages(this->groundDamages, p, ut.groundWeapon().minRange(), ut.groundWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft(), 
            - ut.groundWeapon().damageAmount() * ut.maxGroundHits());
        //updateDamagesGrad(this->groundDamagesGrad, this->groundDamages, p, ut.groundWeapon().minRange(), ut.groundWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft());
    }
    if (ut.airWeapon() != BWAPI::WeaponTypes::None)
    {
        int addRange = additionalRangeAir(ut);
        modifyDamages(this->airDamages, p, ut.airWeapon().minRange(), ut.airWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft(), 
            - ut.airWeapon().damageAmount() * ut.maxAirHits());
        //updateDamagesGrad(this->airDamagesGrad, this->airDamages, p, ut.airWeapon().minRange(), ut.airWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft());
    }
}

void MapManager::removeDmgStorm(Position p)
{
    modifyDamages(this->groundDamages, p, 0, 63, -50);
    modifyDamages(this->airDamages, p, 0, 63, -50);
    //updateDamagesGrad(this->groundDamagesGrad, this->groundDamages, p, 0, 63);
    //updateDamagesGrad(this->airDamagesGrad, this->airDamages, p, 0, 63);
}

void MapManager::addDmg(UnitType ut, Position p)
{ 
    if (ut.groundWeapon() != BWAPI::WeaponTypes::None)
    {
        int addRange = additionalRangeGround(ut);
        modifyDamages(this->groundDamages, p, ut.groundWeapon().minRange(), ut.groundWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft(), 
            ut.groundWeapon().damageAmount() * ut.maxGroundHits());
        //updateDamagesGrad(this->groundDamagesGrad, this->groundDamages, p, ut.groundWeapon().minRange(), ut.groundWeapon().maxRange() + addRange);
    }
    if (ut.airWeapon() != BWAPI::WeaponTypes::None)
    {
        int addRange = additionalRangeAir(ut);
        modifyDamages(this->airDamages, p, ut.airWeapon().minRange(), ut.airWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft(), 
            ut.airWeapon().damageAmount() * ut.maxAirHits());
        //updateDamagesGrad(this->airDamagesGrad, this->airDamages, p, ut.airWeapon().minRange(), ut.airWeapon().maxRange() + addRange);
    }
}

void MapManager::addDmgWithoutGrad(UnitType ut, Position p)
{ 
    if (ut.groundWeapon() != BWAPI::WeaponTypes::None)
    {
        int addRange = additionalRangeGround(ut);
        modifyDamages(this->groundDamages, p, ut.groundWeapon().minRange(), ut.groundWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft(), 
            ut.groundWeapon().damageAmount() * ut.maxGroundHits());
    }
    if (ut.airWeapon() != BWAPI::WeaponTypes::None)
    {
        int addRange = additionalRangeAir(ut);
        modifyDamages(this->airDamages, p, ut.airWeapon().minRange(), ut.airWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft(), 
            ut.airWeapon().damageAmount() * ut.maxAirHits());
    }
}

void MapManager::addDmgStorm(Position p)
{
    // storm don't stack, but we make them stack
    modifyDamages(this->groundDamages, p, 0, 63, 50); 
    modifyDamages(this->airDamages, p, 0, 63, 50);
}

int MapManager::additionalRangeGround(UnitType ut)
{
    // we consider that the upgrades are always researched/up
    if (ut == UnitTypes::Protoss_Dragoon)
        return 64;
    else if (ut == UnitTypes::Terran_Marine)
        return 32;
    else if (ut == UnitTypes::Zerg_Hydralisk)
        return 32;
    else if (ut == UnitTypes::Terran_Vulture_Spider_Mine)
        return 64;
    else 
        return 0;
}

int MapManager::additionalRangeAir(UnitType ut)
{
    // we consider that the upgrades are always researched/up
    if (ut == UnitTypes::Protoss_Dragoon)
        return 64;
    else if (ut == UnitTypes::Terran_Marine)
        return 32;
    else if (ut == UnitTypes::Terran_Goliath)
        return 96;
    else if (ut == UnitTypes::Zerg_Hydralisk)
        return 32;
    else if (ut == UnitTypes::Terran_Missile_Turret 
        || ut == UnitTypes::Zerg_Spore_Colony
        || ut == UnitTypes::Protoss_Photon_Cannon)
        return 32;
    else 
        return 0;
}

void MapManager::onUnitCreate(Unit* u)
{
	if (u->getType().isBuilding())
		addBuilding(u);
    if (u->getPlayer() == Broodwar->self())
        addAlliedUnit(u);
}

void MapManager::onUnitDestroy(Unit* u)
{
    removeBuilding(u);
    if (u->getPlayer() == Broodwar->enemy())
    {
        removeDmg(u->getType(), _trackedUnits[u]);
        _trackedUnits.erase(u);
    }
    else
    {
        removeAlliedUnit(u);
    }
}

void MapManager::onUnitShow(Unit* u)
{
	if (u->getType().isBuilding())
	    addBuilding(u);
	if (u->getPlayer()->isAlly(Broodwar->self()) || u->getPlayer() == Broodwar->self())
        addAlliedUnit(u);
}

void MapManager::onUnitHide(Unit* u)
{
    addBuilding(u);
}

unsigned __stdcall MapManager::StaticLaunchUpdateStormPos(void* obj)
{
    MapManager* This = (MapManager*) obj;
    int buf = This->LaunchUpdateStormPos();
    _endthreadex(buf);
    return buf;
}

DWORD MapManager::LaunchUpdateStormPos()
{
	while (true)
	{
	    DWORD startUpdate = WaitForSingleObject(
	        _signalLaunchStormUpdate,
	        INFINITE);
		if (startUpdate == WAIT_OBJECT_0)
		{
		    DWORD waitResult = WaitForSingleObject(
		        _stormPosMutex,
		        100); // 100 ms, the game would be really slow if it were at 10 FPS just because of MapManager::update() (storm)
			if (waitResult == WAIT_OBJECT_0)
				updateStormPos();
			ReleaseMutex(_stormPosMutex);
		}
	}
    return 0;
}

void MapManager::updateStormPos()
{
    _stormPosBuf.clear();
    // Construct different possible positions for the storms, shifting by __MESH_SIZE__
    std::set<Position> possiblePos;
    for (std::map<BWAPI::Unit*, BWAPI::Position>::const_iterator eit = _enemyUnitsPosBuf.begin();
        eit != _enemyUnitsPosBuf.end(); ++eit)
    {
        Position tmpPos = eit->second;
        if (possiblePos.size() > 32) // TOCHANGE 32 units
            possiblePos.insert(tmpPos);
        else
        {
            possiblePos.insert(tmpPos);
            if (tmpPos.x() >= __MESH_SIZE__)
            {
                possiblePos.insert(Position(tmpPos.x() - __MESH_SIZE__, tmpPos.y()));
                if (tmpPos.y() + __MESH_SIZE__ < _pix_height)
                    possiblePos.insert(Position(tmpPos.x() - __MESH_SIZE__, tmpPos.y() - __MESH_SIZE__));
                if (tmpPos.y() >= __MESH_SIZE__)
                    possiblePos.insert(Position(tmpPos.x() - __MESH_SIZE__, tmpPos.y() + __MESH_SIZE__));
            }
            if (tmpPos.x() + __MESH_SIZE__ < _pix_width)
            {
                possiblePos.insert(Position(tmpPos.x() + __MESH_SIZE__, tmpPos.y()));
                if (tmpPos.y() >= __MESH_SIZE__)
                    possiblePos.insert(Position(tmpPos.x() + __MESH_SIZE__, tmpPos.y() + __MESH_SIZE__));
                if (tmpPos.y() + __MESH_SIZE__ < _pix_height)
                    possiblePos.insert(Position(tmpPos.x() + __MESH_SIZE__, tmpPos.y() - __MESH_SIZE__));
            }
            if (tmpPos.y() >= __MESH_SIZE__) 
                possiblePos.insert(Position(tmpPos.x(), tmpPos.y() - __MESH_SIZE__));
            if (tmpPos.y() + __MESH_SIZE__ < _pix_height)
                possiblePos.insert(Position(tmpPos.x(), tmpPos.y() + __MESH_SIZE__));
        }
    }
    // Score the possible possitions for the storms
    std::set<std::pair<int, Position> > storms;
    for (std::set<Position>::const_iterator it = possiblePos.begin();
        it != possiblePos.end(); ++it)
    {
        int tmp = 0;
        for (std::map<BWAPI::Unit*, BWAPI::Position>::const_iterator uit = _alliedUnitsPosBuf.begin();
            uit != _alliedUnitsPosBuf.end(); ++uit)
        {
            if (uit->second.x() > it->x() - (__STORM_SIZE__ / 2 + 1) && uit->second.x() < it->x() + (__STORM_SIZE__ / 2 + 1)
                && uit->second.y() > it->y() - (__STORM_SIZE__ / 2 + 1) && uit->second.y() < it->y() + (__STORM_SIZE__ / 2 + 1))
                tmp -= 4;
        }
        for (std::map<BWAPI::Unit*, BWAPI::Position>::const_iterator eit = _enemyUnitsPosBuf.begin();
            eit != _enemyUnitsPosBuf.end(); ++eit)
        {
            // TODO TOCHANGE 40 here, it could be 3 tiles x 32  / 2 = 48 or less (to account for their movement)
            if (eit->second.x() > it->x() - 40 && eit->second.x() < it->x() + 40
                && eit->second.y() > it->y() - 40 && eit->second.y() < it->y() + 40)
                tmp += 2;
            // TODO TOCHANGE 8 here, to center, it could be 1 tiles x 32 / 2 = 16 or less
            /*if (eit->second.x() > it->x() - 8 && eit->second.x() < it->x() + 8
                && eit->second.y() > it->y() - 8 && eit->second.y() < it->y() + 8)
                ++tmp;*/
        }
        for (std::map<BWAPI::Unit*, std::pair<BWAPI::UnitType, BWAPI::Position> >::const_iterator iit = _invisibleUnitsBuf.begin();
            iit != _invisibleUnitsBuf.end(); ++ iit)
        {
			if (!(iit->second.second.x() > it->x() - (__STORM_SIZE__ / 2 + 1) && iit->second.second.x() < it->x() + (__STORM_SIZE__ / 2 + 1)
			    && iit->second.second.y() > it->y() - (__STORM_SIZE__ / 2 + 1) && iit->second.second.y() < it->y() + (__STORM_SIZE__ / 2 + 1)))
				continue;
            if (iit->second.first == UnitTypes::Protoss_Reaver 
				|| iit->second.first == UnitTypes::Terran_Siege_Tank_Siege_Mode
				|| iit->second.first == UnitTypes::Zerg_Lurker)
                ++tmp;            
			else if (iit->second.first == UnitTypes::Protoss_Observer || iit->second.first != UnitTypes::Zerg_Zergling
				|| iit->second.first != UnitTypes::Terran_Vulture_Spider_Mine || iit->second.first != UnitTypes::Terran_Vulture)
				--tmp;
        }
        if (tmp > 0)
        {
            storms.insert(std::make_pair<int, Position>(tmp, *it));
        }
    }
    // Filter the positions for the storms by descending order + eliminate some overlapings
    std::set<Position> covered;
    for (std::set<std::pair<int, Position> >::const_reverse_iterator it = storms.rbegin();
        it != storms.rend(); ++it)
    {
        bool foundCoverage = false;
        for (std::set<Position>::const_iterator i = covered.begin();
            i != covered.end(); ++i)
        {
            if (it->second.x() > i->x() - (__STORM_SIZE__ / 2 + 1) && it->second.x() < i->x() + (__STORM_SIZE__ / 2 + 1)
                && it->second.y() > i->y() - (__STORM_SIZE__ / 2 + 1) && it->second.y() < i->y() + (__STORM_SIZE__ / 2 + 1))
            {
                foundCoverage = true;
                break;
            }
        }
        for (std::map<Position, int>::const_iterator i = _dontReStormBuf.begin();
            i != _dontReStormBuf.end(); ++i)
        {
            if (it->second.x() > i->first.x() - (__STORM_SIZE__ / 2 + 1) && it->second.x() < i->first.x() + (__STORM_SIZE__ / 2 + 1)
                && it->second.y() > i->first.y() - (__STORM_SIZE__ / 2 + 1) && it->second.y() < i->first.y() + (__STORM_SIZE__ / 2 + 1))
            {
                foundCoverage = true;
                break;
            }
        }
        if (!foundCoverage)
        {
            _stormPosBuf.insert(std::make_pair<Position, int>(it->second, it->first));
            covered.insert(it->second);
        }
    }
    return;
}

void MapManager::justStormed(Position p)
{
    stormPos.erase(p);
    // remove the >= 4/9 (buildtiles) overlaping storms yet present in stormPos
    for (std::map<Position, int>::iterator it = stormPos.begin();
        it != stormPos.end(); )
    {
        if (it->first.getDistance(p) < 46.0)
        {
            stormPos.erase(it++);
        }
        else
            ++it;
    }
    if (_dontReStorm.count(p))
        _dontReStorm[p] = 0;
    else
        _dontReStorm.insert(std::make_pair<Position, int>(p, 0));
}

void MapManager::update()
{
#ifdef __INTELLIGENCE_DEBUG__
    clock_t start = clock();
	drawLowResWalkability(); /// TODO REMOVE
#endif


    // update our units' positions
    for (std::map<BWAPI::Unit*, BWAPI::Position>::iterator it = _ourUnits.begin();
        it != _ourUnits.end(); ++it)
    {
        _ourUnits[it->first] = it->first->getPosition();
    }

    // check/update the damage maps. BEWARE: hidden units are not removed in presence of doubt!
    std::list<Unit*> toFilter;
    for (std::map<BWAPI::Unit*, EViewedUnit>::const_iterator it = _eUnitsFilter->getViewedUnits().begin();
        it != _eUnitsFilter->getViewedUnits().end(); ++it)
    {
        if (_trackedUnits.count(it->first))
        {
            if (it->first->exists()
                && it->first->isVisible()
                && (_trackedUnits[it->first] != it->first->getPosition())) // it moved
            {
                // update EUnitsFilter
                _eUnitsFilter->update(it->first);
                // update MapManager (ourselves)
                if (it->first->getType().isWorker()
                    && (it->first->isRepairing() || it->first->isConstructing()
                    || it->first->isGatheringGas() || it->first->isGatheringMinerals()))
                {
                    removeDmg(it->first->getType(), _trackedUnits[it->first]);
                    _trackedUnits.erase(it->first);
                }
                else
                {
                    addDmgWithoutGrad(it->first->getType(), it->first->getPosition());
                    removeDmg(it->first->getType(), _trackedUnits[it->first]);
                    _trackedUnits[it->first] = it->first->getPosition();
                }
            }
        }
        else
        {
            if (it->first->exists()
                && it->first->isVisible() // SEGFAULT
                && !(it->first->getType().isWorker()
                && (it->first->isRepairing() || it->first->isConstructing()
                || it->first->isGatheringGas() || it->first->isGatheringMinerals())))
            {
                // add it to MapManager (ourselves)
                addDmg(it->first->getType(), it->first->getPosition());
                _trackedUnits.insert(std::make_pair<Unit*, Position>(it->first, it->first->getPosition()));
            }
        }
        if (!(it->first->isVisible()))
        {
            toFilter.push_back(it->first);
        }
    }
    for (std::map<Unit*, Position>::iterator it = _trackedUnits.begin();
       it != _trackedUnits.end(); )
    {
        if (!it->first->isVisible() && !(_eUnitsFilter->getInvisibleUnits().count(it->first)))
        {
            //Broodwar->printf("removing a %s", _eUnitsFilter->getViewedUnit(it->first).type.getName().c_str());
            removeDmg(_eUnitsFilter->getViewedUnit(it->first).type, _trackedUnits[it->first]);
            _trackedUnits.erase(it++);
        }
        else
            ++it;
    }
    for (std::list<Unit*>::const_iterator it = toFilter.begin();
        it != toFilter.end(); ++it)
    {
        _eUnitsFilter->filter(*it);
    }

    // Iterate of all the Bullets to extract the interesting ones
    for (std::set<Bullet*>::const_iterator it = Broodwar->getBullets().begin();
        it != Broodwar->getBullets().end(); ++it)
    {
        if ((*it)->getType() == BWAPI::BulletTypes::Psionic_Storm)
        {
            if ((*it)->exists() && !_trackedStorms.count(*it))
            {
                _trackedStorms.insert(std::make_pair<Bullet*, Position>(*it, (*it)->getPosition()));
                addDmgStorm((*it)->getPosition());                
            }
        }
    }

    // Updating the damages maps with storms 
    // (overlapping => more damage, that's false but easy AND handy b/c of durations)
    for (std::map<Bullet*, Position>::iterator it = _trackedStorms.begin();
        it != _trackedStorms.end(); )
    {
        if (!it->first->exists())
        {
            removeDmgStorm(it->second);
            _trackedStorms.erase(it++);
        }
        else
            ++it;
    }

    if (Broodwar->self()->hasResearched(BWAPI::TechTypes::Psionic_Storm))
    {
        // update the possible storms positions
        if (WaitForSingleObject(_stormPosMutex, 0) == WAIT_OBJECT_0) // cannot enter when the thread is running
        {
            /// Update stormPos
            stormPos = _stormPosBuf;
            int lastUpdateDiff = Broodwar->getFrameCount() - _lastStormUpdateFrame;
            // decay the "dont re storm" positions + erase
            for (std::map<Position, int>::iterator it = _dontReStorm.begin();
                it != _dontReStorm.end(); )
            {
                if (stormPos.count(it->first))
                    stormPos.erase(it->first);
                _dontReStorm[it->first] = _dontReStorm[it->first] + lastUpdateDiff;
                if (it->second > 72)
                {
                    _dontReStorm.erase(it++);
                }
                else
                    ++it;
            }
            _lastStormUpdateFrame = Broodwar->getFrameCount();

            /// Prepare for the next update of _stormPosBuf thread
            _enemyUnitsPosBuf = HighTemplarUnit::stormableUnits;
            if (!_enemyUnitsPosBuf.empty())
            {
                _alliedUnitsPosBuf = _ourUnits;
                _invisibleUnitsBuf = _eUnitsFilter->getInvisibleUnits();
#ifdef __INTELLIGENCE_DEBUG__
                for (std::map<Unit*, std::pair<UnitType, Position> >::const_iterator ii = _invisibleUnitsBuf.begin();
                    ii != _invisibleUnitsBuf.end(); ++ii)
                    Broodwar->drawCircleMap(ii->second.second.x(), ii->second.second.y(), 8, Colors::Red, true);
#endif
                // Don't restorm where there are already existing storms, lasting more than 48 frames
                for (std::map<Bullet*, Position>::const_iterator it = _trackedStorms.begin();
                    it != _trackedStorms.end(); ++it)
                {
                    if (it->first->exists() && it->first->getRemoveTimer() > 0) // TODO TOCHANGE 47
                    {
                        if (_dontReStorm.count(it->second))
                            _dontReStorm[it->second] = 72 - it->first->getRemoveTimer();
                        else
                            _dontReStorm.insert(std::make_pair<Position, int>(it->second, 72 - it->first->getRemoveTimer()));
                    }
                }
                _dontReStormBuf = _dontReStorm;
                // this thread is doing updateStormPos();
				if (_stormThread == NULL)
				{
	                unsigned threadId;
					_stormThread = (HANDLE)_beginthreadex( 
						NULL,                   // default security attributes
						0,                      // use default stack size  
						&MapManager::StaticLaunchUpdateStormPos,      // thread function name
						(void*) this,                   // argument to thread function 
						0,                      // use default creation flags 
						&threadId);             // returns the thread identifier 
					if (_stormThread == NULL)
					{
						Broodwar->printf("(MapManager) error creating thread");
					}
				}
				else
				{
					/// Launch the update of storms pos by unlocking the thread (waiting on this mutex)
					if (!SetEvent(_signalLaunchStormUpdate))
						Broodwar->printf("(MapManager) error signaling the storm pos update thread");
				}
            }
            else
            {
                _stormPosBuf.clear();
            }
        }
        ReleaseMutex(_stormPosMutex);
    }
	
	/// Update the pathfinder
	if (WaitForSingleObject(_pathfindingMutex, 0) == WAIT_OBJECT_0) // cannot enter when the thread is running
	{
		/// Fetch results if there are some
		if (_currentPathfindWorkAborded)
			_currentPathfindWork.bunit = NULL;
		if (!_currentPathfindWorkAborded 
			&& _currentPathfindWork.bunit != NULL
			&& _currentPathfindWork.bunit->unit != NULL
			&& _currentPathfindWork.bunit->unit == _currentPathfindWork.unit // why do I have to even do this?
			&& _currentPathfindWork.bunit->unit->exists()
			&& !_currentPathfindWork.btpath.empty())
		{
			/*if (_currentPathfindWork.btpath.size() > _currentPathfindWork.bunit->btpath.size())
			{
				_currentPathfindWork.bunit->btpath.clear();
				_currentPathfindWork.bunit->btpath.reserve(_currentPathfindWork.btpath.size());
			}
			copy(_currentPathfindWork.btpath.begin(), _currentPathfindWork.btpath.end(), _currentPathfindWork.bunit->btpath.begin());*/
			_currentPathfindWork.bunit->btpath.swap(_currentPathfindWork.btpath);
			_currentPathfindWork.bunit->newPath();
			_currentPathfindWork.bunit = NULL;
		}
		_currentPathfindWorkAborded = false;

		/// Prepare the next currentPathfindWork
		if (!_pathfindWorks.empty())
		{
			_currentPathfindWork = _pathfindWorks.front();
			_pathfindWorks.pop_front();
			if (_currentPathfindWork.damages != -1)
			{
				if (_currentPathfindWork.bunit->unit->getType().isFlyer())
					memcpy(_airDamagesBuf, airDamages, Broodwar->mapWidth() * Broodwar->mapHeight());
				else
				{
					memcpy(_groundDamagesBuf, groundDamages, Broodwar->mapWidth() * Broodwar->mapHeight());
					memcpy(_buildingsBuf, buildings, Broodwar->mapWidth() * Broodwar->mapHeight());
				}
			}
			else
			{
				memcpy(_buildingsBuf, buildings, Broodwar->mapWidth() * Broodwar->mapHeight());
			}

			/// Create the thread if it doesn't exist
			if (_pathfindingThread == NULL)
			{
				unsigned threadId;
				_pathfindingThread = (HANDLE)_beginthreadex( 
					NULL,                   // default security attributes
					0,                      // use default stack size  
					&MapManager::StaticLaunchPathfinding,      // thread function name
					(void*) this,                   // argument to thread function 
					0,                      // use default creation flags 
					&threadId);             // returns the thread identifier 
				if (_pathfindingThread == NULL)
				{
					Broodwar->printf("(MapManager) error creating thread");
				}
			}
			else
			{
				/// Signal the pathfinder thread of incoming work
				if (!SetEvent(_signalLaunchPathfinding))
					Broodwar->printf("(MapManager) error signaling the pathfinder thread of new work");
			}
		}
		ReleaseMutex(_pathfindingMutex);
	}

#ifdef __INTELLIGENCE_DEBUG__
    clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;
    if (duration > 0.040) 
        Broodwar->printf("MapManager::update() took: %2.5f seconds\n", duration);
    //this->drawGroundDamagesGrad(); // DRAW
    //this->drawGroundDamages();
    //this->drawAirDamagesGrad();
    //this->drawAirDamages();
	if (Intelligence::Instance().enemyHome != NULL)
		drawBTPath(_pathsFromHomeToSL[Intelligence::Instance().enemyHome]);
    this->drawBestStorms();
	//this->drawWalkability();
#endif
}

const std::map<BWAPI::Unit*, BWAPI::Position>& MapManager::getOurUnits()
{
    return _ourUnits;
}

const std::map<BWAPI::Unit*, BWAPI::Position>& MapManager::getTrackedUnits()
{
    return _trackedUnits;
}

const std::map<BWAPI::Bullet*, BWAPI::Position>& MapManager::getTrackedStorms()
{
    return _trackedStorms;
}

Position MapManager::closestWalkableSameRegionOrConnected(Position p)
{
    if (!p.isValid())
        p.makeValid();
    WalkTilePosition wtp(p);
    TilePosition tp(p);
    BWTA::Region* r = BWTA::getRegion(tp);
    int lowerX = (wtp.x() - 1) > 0 ? wtp.x() - 1 : 0;
    int higherX = (wtp.x() + 1) < _width ? wtp.x() + 1 : _width;
    int lowerY = (wtp.y() - 1) > 0 ? wtp.y() - 1 : 0;
    int higherY = (wtp.y() + 1) < _height ? wtp.y() + 1 : _height;
    Position saved = Positions::None;
    for (int x = lowerX; x <= higherX; ++x)
    {
        for (int y = lowerY; y <= higherY; ++y)
        {
            if (walkability[x + y*_width])
            {
                if (BWTA::getRegion(x/4, y/4) == r)
                    return WalkTilePosition(x, y).getPosition();
                else if (BWTA::isConnected(TilePosition(x/4, y/4), tp))
                    saved = WalkTilePosition(x, y).getPosition();
            }
        }
    }
    if (saved != Positions::None)
        return saved;
    // else, quickly (so, approximately, not as exact as in the method name :))
    lowerX = (wtp.x() - 4) > 0 ? wtp.x() - 4 : 0;
    higherX = (wtp.x() + 4) < _width ? wtp.x() + 4 : _width;
    lowerY = (wtp.y() - 4) > 0 ? wtp.y() - 4 : 0;
    higherY = (wtp.y() + 4) < _height ? wtp.y() + 4 : _height;
    for (int x = lowerX; x <= higherX; ++x)
    {
        for (int y = lowerY; y <= higherY; ++y)
        {
            if (walkability[x + y*_width])
            {
                if (BWTA::getRegion(x/4, y/4) == r)
                    return WalkTilePosition(x, y).getPosition();
                else if (BWTA::isConnected(TilePosition(x/4, y/4), tp))
                    saved = WalkTilePosition(x, y).getPosition();
            }
        }
    }
    if (saved != Positions::None)
        return saved;
    return Positions::None;
}

TilePosition MapManager::closestWalkable(TilePosition tp, BWTA::Region* r)
{
#ifdef __INTELLIGENCE_DEBUG__
	assert(tp.isValid());
	assert(r != NULL);
#endif
    if (!tp.isValid())
        tp.makeValid();
	if (r == NULL) // defensive
		return tp;
    int lowerX = (tp.x() - 1) > 0 ? tp.x() - 1 : 0;
    int higherX = (tp.x() + 1) < Broodwar->mapWidth() ? tp.x() + 1 : Broodwar->mapWidth();
    int lowerY = (tp.y() - 1) > 0 ? tp.y() - 1 : 0;
    int higherY = (tp.y() + 1) < Broodwar->mapHeight() ? tp.y() + 1 : Broodwar->mapHeight();
    TilePosition saved = TilePositions::None;
    for (int x = lowerX; x <= higherX; ++x)
    {
        for (int y = lowerY; y <= higherY; ++y)
        {
#ifdef __INTELLIGENCE_DEBUG__
            //Broodwar->drawBoxMap(x*32 + 2, y*32 + 2, x*32+29, y*32+29, Colors::Red);
#endif
            if (_lowResWalkability[x + y*Broodwar->mapWidth()])
            {
                if (BWTA::getRegion(x, y) == r)
                    return TilePosition(x, y);
                else if (BWTA::isConnected(TilePosition(x, y), tp))
                    saved = TilePosition(x, y);
            }
        }
    }
    if (saved != TilePositions::None)
        return saved;
    // else, quickly (so, approximately, not as exact as in the method name :))
    lowerX = (tp.x() - 2) > 0 ? tp.x() - 2 : 0;
    higherX = (tp.x() + 2) < Broodwar->mapWidth() ? tp.x() + 2 : Broodwar->mapWidth();
    lowerY = (tp.y() - 2) > 0 ? tp.y() - 2 : 0;
    higherY = (tp.y() + 2) < Broodwar->mapHeight() ? tp.y() + 2 : Broodwar->mapHeight();
    for (int x = lowerX; x <= higherX; ++x)
    {
        for (int y = lowerY; y <= higherY; ++y)
        {
#ifdef __INTELLIGENCE_DEBUG__
            //Broodwar->drawBoxMap(x*32 + 2, y*32 + 2, x*32+29, y*32+29, Colors::Red);
#endif
            if (_lowResWalkability[x + y*Broodwar->mapWidth()])
            {
                if (BWTA::getRegion(x, y) == r)
                    return TilePosition(x, y);
                else if (BWTA::isConnected(TilePosition(x, y), tp))
                    saved = TilePosition(x, y);
            }
        }
    }
    if (saved != TilePositions::None)
        return saved;
    return TilePositions::None;
}

TilePosition MapManager::closestWalkableSameRegionOrConnected(TilePosition tp)
{
#ifdef __INTELLIGENCE_DEBUG__
	assert(tp.isValid());
#endif
    if (!tp.isValid())
        tp.makeValid();
    BWTA::Region* r = BWTA::getRegion(tp);
	return closestWalkable(tp, r);
}

bool MapManager::isBTWalkable(int x, int y)
{
	return _lowResWalkability[x + y*Broodwar->mapWidth()] 
		   && !buildings[x + y*Broodwar->mapWidth()];
}

bool MapManager::isBTWalkable(const TilePosition& tp)
{
	return _lowResWalkability[tp.x() + tp.y()*Broodwar->mapWidth()] 
		   && !buildings[tp.x() + tp.y()*Broodwar->mapWidth()];
}

void MapManager::drawBuildings()
{
    for (int x = 0; x < _width; ++x)
        for (int y = 0; y < _height; ++y)
        {
            if (buildings_wt[x + y*_width])
                Broodwar->drawBoxMap(8*x+1, 8*y+1, 8*x+7, 8*y+7, Colors::Orange);
        }
}

#ifdef __BUILDINGS_WT_STRICT__
void MapManager::drawBuildingsStrict()
{
    for (int x = 0; x < _width; ++x)
        for (int y = 0; y < _height; ++y)
        {
            if (buildings_wt_strict[x + y*_width])
                Broodwar->drawBoxMap(8*x+1, 8*y+1, 8*x+7, 8*y+7, Colors::Orange);
        }
}
#endif

void MapManager::drawWalkability()
{
    for (int x = 0; x < _width; ++x)
        for (int y = 0; y < _height; ++y)
        {
            if (!walkability[x + y*_width])
                Broodwar->drawBox(CoordinateType::Map, 8*x + 1, 8*y + 1, 8*x + 7, 8*y + 7, Colors::Red);
        }
}

void MapManager::drawLowResWalkability()
{
    for (int x = 0; x < Broodwar->mapWidth(); ++x)
        for (int y = 0; y < Broodwar->mapHeight(); ++y)
        {
            if (!_lowResWalkability[x + y*Broodwar->mapWidth()])
                Broodwar->drawBox(CoordinateType::Map, 32*x + 2, 32*y + 2, 32*x + 30, 32*y + 30, Colors::Red);
        }
}

void MapManager::drawLowResBuildings()
{
    for (int x = 0; x < Broodwar->mapWidth(); ++x)
        for (int y = 0; y < Broodwar->mapHeight(); ++y)
        {
            if (buildings[x + y*Broodwar->mapWidth()])
                Broodwar->drawBox(CoordinateType::Map, 32*x + 2, 32*y + 2, 32*x + 30, 32*y + 30, Colors::Orange);
            //Broodwar->drawBox(CoordinateType::Map, 32*x - 15, 32*y - 15, 32*x + 15, 32*y + 15, Colors::Orange);
        }
}

void MapManager::drawGroundDamages()
{
    for (int x = 0; x < Broodwar->mapWidth(); ++x)
        for (int y = 0; y < Broodwar->mapHeight(); ++y)
        {
            if (groundDamages[x + y*Broodwar->mapWidth()] > 0 && groundDamages[x + y*Broodwar->mapWidth()] <= 40)
            {
                Broodwar->drawBox(CoordinateType::Map, 32*x - 14, 32*y - 14, 32*x +14, 32*y +14, Colors::White);
            }
            else if (groundDamages[x + y*Broodwar->mapWidth()] > 40 && groundDamages[x + y*Broodwar->mapWidth()] <= 80)
            {
                Broodwar->drawBox(CoordinateType::Map, 32*x - 14, 32*y - 14, 32*x +14, 32*y +14, Colors::Yellow);
            }
            else if (groundDamages[x + y*Broodwar->mapWidth()] > 80 && groundDamages[x + y*Broodwar->mapWidth()] <= 160)
            {
                Broodwar->drawBox(CoordinateType::Map, 32*x - 14, 32*y - 14, 32*x +14, 32*y +14, Colors::Orange);
            }
            else if (groundDamages[x + y*Broodwar->mapWidth()] > 160)
            {
                Broodwar->drawBox(CoordinateType::Map, 32*x - 14, 32*y - 14, 32*x +14, 32*y +14, Colors::Red);
            }
        }
}

void MapManager::drawAirDamages()
{
    for (int x = 0; x < Broodwar->mapWidth(); ++x)
        for (int y = 0; y < Broodwar->mapHeight(); ++y)
        {
            if (airDamages[x + y*Broodwar->mapWidth()] > 0 && airDamages[x + y*Broodwar->mapWidth()] <= 40)
            {
                Broodwar->drawBox(CoordinateType::Map, 32*x - 14, 32*y - 14, 32*x +14, 32*y +14, Colors::White);
            }
            else if (airDamages[x + y*Broodwar->mapWidth()] > 40 && airDamages[x + y*Broodwar->mapWidth()] <= 80)
            {
                Broodwar->drawBox(CoordinateType::Map, 32*x - 14, 32*y - 14, 32*x +14, 32*y +14, Colors::Yellow);
            }
            else if (airDamages[x + y*Broodwar->mapWidth()] > 80 && airDamages[x + y*Broodwar->mapWidth()] <= 160)
            {
                Broodwar->drawBox(CoordinateType::Map, 32*x - 14, 32*y - 14, 32*x +14, 32*y +14, Colors::Orange);
            }
            else if (airDamages[x + y*Broodwar->mapWidth()] > 160)
            {
                Broodwar->drawBox(CoordinateType::Map, 32*x - 14, 32*y - 14, 32*x +14, 32*y +14, Colors::Red);
            }
        }
}

void MapManager::drawGroundDamagesGrad()
{
    for (int x = 0; x < Broodwar->mapWidth(); ++x)
        for (int y = 0; y < Broodwar->mapHeight(); ++y)
        {
            Vec tmp = groundDamagesGrad[x + y*Broodwar->mapWidth()];
            tmp.normalize();
            tmp *= 14;
            Position tmpPos = Position(x*32, y*32);
            if (groundDamagesGrad[x + y*Broodwar->mapWidth()].norm() > 0 && groundDamagesGrad[x + y*Broodwar->mapWidth()].norm() <= 40)
            {
                Broodwar->drawLineMap(tmpPos.x(), tmpPos.y(), tmp.translate(tmpPos).x(), tmp.translate(tmpPos).y(), Colors::White);
            }
            else if (groundDamagesGrad[x + y*Broodwar->mapWidth()].norm() > 40 && groundDamagesGrad[x + y*Broodwar->mapWidth()].norm() <= 80)
            {
                Broodwar->drawLineMap(tmpPos.x(), tmpPos.y(), tmp.translate(tmpPos).x(), tmp.translate(tmpPos).y(), Colors::Yellow);
            }
            else if (groundDamagesGrad[x + y*Broodwar->mapWidth()].norm() > 80 && groundDamagesGrad[x + y*Broodwar->mapWidth()].norm() <= 160)
            {
                Broodwar->drawLineMap(tmpPos.x(), tmpPos.y(), tmp.translate(tmpPos).x(), tmp.translate(tmpPos).y(), Colors::Orange);
            }
            else if (groundDamagesGrad[x + y*Broodwar->mapWidth()].norm() > 160)
            {
                Broodwar->drawLineMap(tmpPos.x(), tmpPos.y(), tmp.translate(tmpPos).x(), tmp.translate(tmpPos).y(), Colors::Red);
            }
        }
}

void MapManager::drawAirDamagesGrad()
{
    for (int x = 0; x < Broodwar->mapWidth(); ++x)
        for (int y = 0; y < Broodwar->mapHeight(); ++y)
        {
            Vec tmp = airDamagesGrad[x + y*Broodwar->mapWidth()];
            tmp.normalize();
            tmp *= 14;
            Position tmpPos = Position(x*32, y*32);
            if (airDamagesGrad[x + y*Broodwar->mapWidth()].norm() > 0 && airDamagesGrad[x + y*Broodwar->mapWidth()].norm() <= 40)
            {
                Broodwar->drawLineMap(tmpPos.x(), tmpPos.y(), tmp.translate(tmpPos).x(), tmp.translate(tmpPos).y(), Colors::White);
            }
            else if (airDamagesGrad[x + y*Broodwar->mapWidth()].norm() > 40 && airDamagesGrad[x + y*Broodwar->mapWidth()].norm() <= 80)
            {
                Broodwar->drawLineMap(tmpPos.x(), tmpPos.y(), tmp.translate(tmpPos).x(), tmp.translate(tmpPos).y(), Colors::Yellow);
            }
            else if (airDamagesGrad[x + y*Broodwar->mapWidth()].norm() > 80 && airDamagesGrad[x + y*Broodwar->mapWidth()].norm() <= 160)
            {
                Broodwar->drawLineMap(tmpPos.x(), tmpPos.y(), tmp.translate(tmpPos).x(), tmp.translate(tmpPos).y(), Colors::Orange);
            }
            else if (airDamagesGrad[x + y*Broodwar->mapWidth()].norm() > 160)
            {
                Broodwar->drawLineMap(tmpPos.x(), tmpPos.y(), tmp.translate(tmpPos).x(), tmp.translate(tmpPos).y(), Colors::Red);
            }
        }
}

void MapManager::drawBestStorms()
{
    for (std::map<Position, int>::const_iterator it = stormPos.begin();
        it != stormPos.end(); ++it)
    {
        Broodwar->drawBoxMap(it->first.x() - 48, it->first.y() - 48, it->first.x() + 48, it->first.y() + 48, BWAPI::Colors::Purple);
        char score[5];
        sprintf_s(score, "%d", it->second);
        Broodwar->drawTextMap(it->first.x() + 46, it->first.y() + 46, score);
    }
}

////// Pathfinding stuff //////

const std::vector<BWAPI::TilePosition>& MapManager::getPathFromHomeToSL(BWTA::BaseLocation* b)
{
	return _pathsFromHomeToSL[b];
}

/// Helper function for registering a building aware pathfinding task
void MapManager::pathfind(BayesianUnit* ptr, BWAPI::Unit* u, BWAPI::TilePosition start, BWAPI::TilePosition end)
{
	registerPathfindWork(ptr, u, start, end, -1);
}

/// Helper function for registering a damages aware pathfinding task (NOT buildings aware, perhaps change...)
void MapManager::threatAwarePathfind(BayesianUnit* ptr, BWAPI::Unit* u, BWAPI::TilePosition start, BWAPI::TilePosition end, int damages)
{
	registerPathfindWork(ptr, u, start, end, damages);
}

/// Removes all the pathfinding tasks from this unit, being it in the queue or the current task
/// /!\ It is important to cancelPathfind when a BayesianUnit dies
void MapManager::cancelPathfind(BayesianUnit* ptr)
{
	for (std::list<PathfindWork>::const_iterator it = _pathfindWorks.begin();
		it != _pathfindWorks.end(); )
	{
		if (it->bunit == ptr)
			_pathfindWorks.erase(it++); // it's defensive not to use break here
		else
			++it;
	}
	if (_currentPathfindWork.bunit == ptr)
		_currentPathfindWorkAborded = true;
}

/// Fills the PathfindWork struct with the appropriate parameters for the pathfinding thread to work on
void MapManager::registerPathfindWork(BayesianUnit* ptr, BWAPI::Unit* u, BWAPI::TilePosition start, BWAPI::TilePosition end, int damages)
{
	TilePosition target(end);
	if (!u->getType().isFlyer())
	{
		if (!_lowResWalkability[end.x() + end.y()*Broodwar->mapWidth()])
			target = closestWalkableSameRegionOrConnected(target);
	}
	if (!target.isValid())
		target.makeValid();
	for (std::list<PathfindWork>::const_iterator it = _pathfindWorks.begin();
		it != _pathfindWorks.end(); ++it)
	{
		if (it->bunit == ptr)
		{
			_pathfindWorks.erase(it);
			break;
		}
	}
	_pathfindWorks.push_back(PathfindWork(ptr, u, start, target, damages, u->getType().isFlyer()));
	if (_pathfindWorks.size() > __MAX_PATHFINDING_WORKS__)
	{
		_pathfindWorks.pop_front();
#ifdef __INTELLIGENCE_DEBUG__
		Broodwar->printf("!!! poping front of pathfinding works");
#endif
	}
}

unsigned __stdcall MapManager::StaticLaunchPathfinding(void* obj)
{
    MapManager* This = (MapManager*) obj;
    int buf = This->LaunchPathfinding();
    ExitThread(buf);
    return buf;
}

DWORD MapManager::LaunchPathfinding()
{
	while (true)
	{
	    DWORD startUpdate = WaitForSingleObject(
	        _signalLaunchPathfinding,
	        INFINITE);
		if (startUpdate == WAIT_OBJECT_0)
		{
		    DWORD waitResult = WaitForSingleObject(
				_pathfindingMutex,
		        100); // 100 ms, the game would be really slow if it were at 10 FPS just because of MapManager::update() (pathfinding)
			if (waitResult == WAIT_OBJECT_0)
			{
				if (_currentPathfindWork.damages == -1)
				{
					buildingsAwarePathFind(_currentPathfindWork.btpath,
						_currentPathfindWork.start, _currentPathfindWork.end);
				}
				else
				{
					if (_currentPathfindWork.flyer)
						damagesAwarePathFindAir(_currentPathfindWork.btpath,
						_currentPathfindWork.start, _currentPathfindWork.end, _currentPathfindWork.damages);
					else
						damagesAwarePathFindGround(_currentPathfindWork.btpath,
						_currentPathfindWork.start, _currentPathfindWork.end, _currentPathfindWork.damages);
				}
			}
			ReleaseMutex(_pathfindingMutex);
		}
	}
    return 0;
}

/*
/// TODO change this to use geometry (faster) and direct lines. 
/// TODO do not use walkability without copying first in a thread
void MapManager::pathFind(std::vector<WalkTilePosition>& path, 
                          const Position& p_start, const Position& p_end)
{
    path.clear();
    WalkTilePosition start(p_start);
    WalkTilePosition end(p_end);
    std::multimap<int, WalkTilePosition> openTiles;
    openTiles.insert(std::make_pair(0, start));
    std::map<WalkTilePosition, int> gmap;
    std::map<WalkTilePosition, WalkTilePosition> parent;
    std::set<WalkTilePosition> closedTiles;
    gmap[start]=0;
    parent[start]=start;
    while(!openTiles.empty())
    {
        WalkTilePosition p = openTiles.begin()->second;
        if (p==end)
        {
            std::vector<WalkTilePosition> reverse_path;
            while(p!=parent[p])
            {
                reverse_path.push_back(p);
                p=parent[p];
            }
            reverse_path.push_back(start);
            for(int i=reverse_path.size()-1; i>=0; i--)
                path.push_back(reverse_path[i]);
            return;
        }
        int fvalue = openTiles.begin()->first;
        int gvalue=gmap[p];
        openTiles.erase(openTiles.begin());
        closedTiles.insert(p);
        const int width = 4*BWAPI::Broodwar->mapWidth();
        int minx = max(p.x() - 1, 0);
        int maxx = min(p.x() + 1, width);
        int miny = max(p.y() - 1, 0);
        int maxy = min(p.y() + 1, 4*BWAPI::Broodwar->mapHeight());
        for(int x = minx; x <= maxx; x++)
            for(int y = miny; y <= maxy; y++)
            {
                if (walkability[x + y*width]) continue;
                if (!(p.x() == x || p.y() == y) && 
                    !(walkability[p.x() + y*width] 
                        || walkability[x + p.y()*width])) continue;

                WalkTilePosition t(x,y);
                if (closedTiles.find(t)!=closedTiles.end()) continue;

                int g=gvalue+10;
                if (x != p.x() && y != p.y()) g+=4;
                int dx=abs(x-end.x());
                int dy=abs(y-end.y());
                int h=abs(dx-dy)*10+min(dx,dy)*14;
                int f=g+h;
                if (gmap.find(t)==gmap.end() || g<gmap.find(t)->second)
                {
                    gmap[t]=g;
                    std::pair<std::multimap<int, WalkTilePosition>::iterator, 
                        std::multimap<int, WalkTilePosition>::iterator> itp 
                        = openTiles.equal_range(f);
                    if (itp.second == itp.first) 
                        openTiles.insert(std::make_pair(f, t));
                    else {
                        for (std::multimap<int, WalkTilePosition>
                            ::const_iterator it = itp.first;
                            it != itp.second; ++it) 
                        {
                            if (it->second == t) 
                            {
                                openTiles.erase(it);
                                break;
                            } 
                        }
                        openTiles.insert(std::make_pair(f, t));
                    }
                    parent[t]=p;
                }
            }
    }
    std::vector<WalkTilePosition> nopath;
    path = nopath;
    return;
}
*/

void MapManager::buildingsAwarePathFind(std::vector<TilePosition>& btpath, 
                          const TilePosition& start, const TilePosition& end)
{
    btpath.clear();
    std::multimap<int, TilePosition> openTiles;
    openTiles.insert(std::make_pair(0, start));
    std::map<TilePosition, int> gmap;
    std::map<TilePosition, TilePosition> parent;
    std::set<TilePosition> closedTiles;
    gmap[start]=0;
    parent[start]=start;
    while(!openTiles.empty())
    {
        TilePosition p = openTiles.begin()->second;
        if (p==end)
        {
            std::vector<TilePosition> reverse_path;
            while(p!=parent[p])
            {
                reverse_path.push_back(p);
                p=parent[p];
            }
            reverse_path.push_back(start);
            for(int i=reverse_path.size()-1; i>=0; --i)
                btpath.push_back(reverse_path[i]);
            return;
        }
        int fvalue = openTiles.begin()->first;
        int gvalue=gmap[p];
        openTiles.erase(openTiles.begin());
        closedTiles.insert(p);
        const int width = BWAPI::Broodwar->mapWidth();
        int minx = max(p.x() - 1, 0);
        int maxx = min(p.x() + 1, width - 1);
        int miny = max(p.y() - 1, 0);
        int maxy = min(p.y() + 1, BWAPI::Broodwar->mapHeight() - 1);
        for(int x = minx; x <= maxx; x++)
            for(int y = miny; y <= maxy; y++)
            {
                if (!_lowResWalkability[x + y*width]) continue;
                if (_buildingsBuf[x + y*width]) continue;        // buildingsAware
                if (p.x() != x && p.y() != y && 
                    !_lowResWalkability[p.x() + y*width] 
                    && !_lowResWalkability[x + p.y()*width]
                    && _buildingsBuf[p.x() + y*width]            // buildingsAware
                    && _buildingsBuf[x + p.y()*width]) continue; // buildingsAware

                TilePosition t(x,y);
                if (closedTiles.find(t) != closedTiles.end()) continue;

                int g=gvalue+10;
                if (x != p.x() && y != p.y()) g+=4;
                int dx=abs(x-end.x());
                int dy=abs(y-end.y());
                int h=abs(dx-dy)*10+min(dx,dy)*14;
                int f=g+h;
                if (gmap.find(t)==gmap.end() || g<gmap.find(t)->second)
                {
                    gmap[t]=g;
                    std::pair<std::multimap<int, TilePosition>::iterator, 
                        std::multimap<int, TilePosition>::iterator> itp 
                        = openTiles.equal_range(f);
                    if (itp.second == itp.first) 
                        openTiles.insert(std::make_pair(f, t));
                    else 
                    {
                        for (std::multimap<int, TilePosition>
                            ::const_iterator it = itp.first;
                            it != itp.second; ++it) 
                        {
                            if (it->second == t) 
                            {
                                openTiles.erase(it);
                                break;
                            } 
                        }
                        openTiles.insert(std::make_pair(f, t));
                    }
                    parent[t]=p;
                }
            }
    }
    // empty path
    return;
}

void MapManager::damagesAwarePathFindAir(std::vector<TilePosition>& btpath, 
                          const TilePosition& start, const TilePosition& end,
						  int damagesThreshold)
{
    btpath.clear();
    std::multimap<int, TilePosition> openTiles;
    openTiles.insert(std::make_pair(0, start));
    std::map<TilePosition, int> gmap;
    std::map<TilePosition, TilePosition> parent;
    std::set<TilePosition> closedTiles;
    gmap[start]=0;
    parent[start]=start;
    while(!openTiles.empty())
    {
        TilePosition p = openTiles.begin()->second;
        if (p==end)
        {
            std::vector<TilePosition> reverse_path;
            while(p!=parent[p])
            {
                reverse_path.push_back(p);
                p=parent[p];
            }
            reverse_path.push_back(start);
            for(int i=reverse_path.size()-1; i>=0; --i)
                btpath.push_back(reverse_path[i]);
            return;
        }
        int fvalue = openTiles.begin()->first;
        int gvalue=gmap[p];
        openTiles.erase(openTiles.begin());
        closedTiles.insert(p);
        const int width = BWAPI::Broodwar->mapWidth();
        int minx = max(p.x() - 1, 0);
        int maxx = min(p.x() + 1, width - 1);
        int miny = max(p.y() - 1, 0);
        int maxy = min(p.y() + 1, BWAPI::Broodwar->mapHeight() - 1);
        for(int x = minx; x <= maxx; x++)
            for(int y = miny; y <= maxy; y++)
            {
				if (_airDamagesBuf[x + y*width] > damagesThreshold) continue;                // damagesAware
                if (p.x() != x && p.y() != y
                    && _airDamagesBuf[p.x() + y*width] > damagesThreshold                    // damagesAware
                    && _airDamagesBuf[x + p.y()*width] > damagesThreshold)                   // damagesAware
                    continue;

                TilePosition t(x,y);
                if (closedTiles.find(t) != closedTiles.end()) continue;

                int g=gvalue+10;
                if (x != p.x() && y != p.y()) g+=4;
                int dx=abs(x-end.x());
                int dy=abs(y-end.y());
                int h=abs(dx-dy)*10+min(dx,dy)*14;
                int f=g+h;
                if (gmap.find(t)==gmap.end() || g<gmap.find(t)->second)
                {
                    gmap[t]=g;
                    std::pair<std::multimap<int, TilePosition>::iterator, 
                        std::multimap<int, TilePosition>::iterator> itp 
                        = openTiles.equal_range(f);
                    if (itp.second == itp.first) 
                        openTiles.insert(std::make_pair(f, t));
                    else 
                    {
                        for (std::multimap<int, TilePosition>
                            ::const_iterator it = itp.first;
                            it != itp.second; ++it) 
                        {
                            if (it->second == t) 
                            {
                                openTiles.erase(it);
                                break;
                            } 
                        }
                        openTiles.insert(std::make_pair(f, t));
                    }
                    parent[t]=p;
                }
            }
    }
    // empty path
    return;
}

/// Buildings and damages aware pathfinding
void MapManager::damagesAwarePathFindGround(std::vector<TilePosition>& btpath, 
                          const TilePosition& start, const TilePosition& end,
						  int damagesThreshold)
{
    btpath.clear();
    std::multimap<int, TilePosition> openTiles;
    openTiles.insert(std::make_pair(0, start));
    std::map<TilePosition, int> gmap;
    std::map<TilePosition, TilePosition> parent;
    std::set<TilePosition> closedTiles;
    gmap[start]=0;
    parent[start]=start;
    while(!openTiles.empty())
    {
        TilePosition p = openTiles.begin()->second;
        if (p==end)
        {
            std::vector<TilePosition> reverse_path;
            while(p!=parent[p])
            {
                reverse_path.push_back(p);
                p=parent[p];
            }
            reverse_path.push_back(start);
            for(int i=reverse_path.size()-1; i>=0; --i)
                btpath.push_back(reverse_path[i]);
            return;
        }
        int fvalue = openTiles.begin()->first;
        int gvalue=gmap[p];
        openTiles.erase(openTiles.begin());
        closedTiles.insert(p);
        const int width = BWAPI::Broodwar->mapWidth();
        int minx = max(p.x() - 1, 0);
        int maxx = min(p.x() + 1, width - 1);
        int miny = max(p.y() - 1, 0);
        int maxy = min(p.y() + 1, BWAPI::Broodwar->mapHeight() - 1);
        for(int x = minx; x <= maxx; x++)
            for(int y = miny; y <= maxy; y++)
            {
                if (!_lowResWalkability[x + y*width]) continue;
				if (_groundDamagesBuf[x + y*width] > damagesThreshold) continue;                // damagesAware
                if (_buildingsBuf[x + y*width]) continue;        // buildingsAware
                if (p.x() != x && p.y() != y && 
                    !_lowResWalkability[p.x() + y*width] 
                    && !_lowResWalkability[x + p.y()*width]
                    && _groundDamagesBuf[p.x() + y*width] > damagesThreshold                    // damagesAware
                    && _groundDamagesBuf[x + p.y()*width] > damagesThreshold)                   // damagesAware
					continue;
                if (p.x() != x && p.y() != y && 
                    !_lowResWalkability[p.x() + y*width] 
                    && !_lowResWalkability[x + p.y()*width]
                    && _buildingsBuf[p.x() + y*width]            // buildingsAware
                    && _buildingsBuf[x + p.y()*width])           // buildingsAware
						continue;

                TilePosition t(x,y);
                if (closedTiles.find(t) != closedTiles.end()) continue;

                int g=gvalue+10;
                if (x != p.x() && y != p.y()) g+=4;
                int dx=abs(x-end.x());
                int dy=abs(y-end.y());
                int h=abs(dx-dy)*10+min(dx,dy)*14;
                int f=g+h;
                if (gmap.find(t)==gmap.end() || g<gmap.find(t)->second)
                {
                    gmap[t]=g;
                    std::pair<std::multimap<int, TilePosition>::iterator, 
                        std::multimap<int, TilePosition>::iterator> itp 
                        = openTiles.equal_range(f);
                    if (itp.second == itp.first) 
                        openTiles.insert(std::make_pair(f, t));
                    else 
                    {
                        for (std::multimap<int, TilePosition>
                            ::const_iterator it = itp.first;
                            it != itp.second; ++it) 
                        {
                            if (it->second == t) 
                            {
                                openTiles.erase(it);
                                break;
                            } 
                        }
                        openTiles.insert(std::make_pair(f, t));
                    }
                    parent[t]=p;
                }
            }
    }
    // empty path
    return;
}

/// trash?
void MapManager::straightLine(std::vector<TilePosition>& btpath, 
        const TilePosition& start, const TilePosition& end)
{
    // it does NOT clean btpath for you
    if (start.x() == end.x() && start.y() == end.y()) return;
    TilePosition current = start;
    btpath.push_back(current);
    Position p_end = Position(end);
    while (Position(current).getDistance(p_end) > 31) // 31 for BuildTile Resolution
    {
        Vec line = Vec(end.x() - current.x(), 
            end.y() - current.y());
        if (line.y == 0.0)
        {
            int currentx = (line.x > 0.0 ? current.x() + 1 : current.x() - 1);
            current = TilePosition(currentx, current.y());
            btpath.push_back(current);
            continue;
        }
        double div = abs(line.x / line.y); // line.y != 0.0
        if (div > 2.0)
        {
            int currentx = (line.x > 0.0 ? current.x() + 1 : current.x() - 1);
            current = TilePosition(currentx, current.y());
        } 
        else if (div < 0.5)
        {
            int currenty = (line.y > 0.0 ? current.y() + 1 : current.y() - 1);
            current = TilePosition(current.x(), currenty);
        }
        else
        {
            int currentx = (line.x > 0.0 ? current.x() + 1 : current.x() - 1);
            int currenty = (line.y > 0.0 ? current.y() + 1 : current.y() - 1);
            current = TilePosition(currentx, currenty);
        }
        btpath.push_back(current);
    }
}

/*
/// trash?
void MapManager::quickPathFind(std::vector<TilePosition>& btpath, 
                                        const TilePosition& start, 
                                        const TilePosition& end)
{
    btpath.clear();
    BWTA::Region* r_begin = BWTA::getRegion(start);
    BWTA::Region* r_end = BWTA::getRegion(end);
    if (!r_begin || !r_end) return; // defensive pgming w.r.t. BWTA
    if (r_begin == r_end)
    {
        btpath = BWTA::getShortestPath(start, end);
        return;
    }
    if (!r_end->isReachable(r_begin)) return;

    // only 2 regions
    std::set<BWTA::Chokepoint*> chokes = r_begin->getChokepoints();
    double dmin = DBL_MAX;
    TilePosition checkpoint;
    BWTA::Region* r_next;
    for (std::set<BWTA::Chokepoint*>::const_iterator it = chokes.begin();
        it != chokes.end(); ++it)
    {
        if (BWTA::getGroundDistance(TilePosition((*it)->getCenter()), end) < dmin) // to change?
        {
            checkpoint = TilePosition((*it)->getCenter());
            r_next = ((*it)->getRegions().first == r_begin ? 
                (*it)->getRegions().second : (*it)->getRegions().first);
        }
    }
    btpath = BWTA::getShortestPath(start, checkpoint);
    chokes = r_next->getChokepoints();
    dmin = DBL_MAX;
    TilePosition checkpoint2;
    for (std::set<BWTA::Chokepoint*>::const_iterator it = chokes.begin();
        it != chokes.end(); ++it)
    {
        if (BWTA::getGroundDistance(TilePosition((*it)->getCenter()), end) < dmin) // to change?
            checkpoint2 = TilePosition((*it)->getCenter());
    }
    std::vector<TilePosition> temp_path = BWTA::getShortestPath(checkpoint, checkpoint2);
    for (std::vector<TilePosition>::const_iterator it = temp_path.begin();
        it != temp_path.end(); ++it)
        btpath.push_back(*it);
    return;
}
*/

BWAPI::Position MapManager::regionsPFCenters(BWTA::Region* r)
{
	int tmp = hash(r);
	return Position(_pfMaps.regionsPFCenters[tmp].first, _pfMaps.regionsPFCenters[tmp].second);
}

double MapManager::distRegions(BWTA::Region *r1, BWTA::Region *r2)
{
	return _pfMaps.distRegions[hash(r1)][hash(r2)];
}

double MapManager::distBaseToBase(BWTA::BaseLocation* b1, BWTA::BaseLocation* b2)
{
	return _pfMaps.distBaseToBase[hash(b1)][hash(b2)];
}