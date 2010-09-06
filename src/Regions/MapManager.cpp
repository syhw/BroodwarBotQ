#pragma once
#include <Defines.h>
#include "MapManager.h"
#include "UnitGroupManager.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "HighTemplarUnit.h"

#define __MESH_SIZE__ 32 // 16 // 24 // 32 // 48
#define __STORM_SIZE__ 96

using namespace BWAPI;

std::map<BWAPI::Unit*, BWAPI::Position> HighTemplarUnit::stormableUnits;

MapManager::MapManager()
: _pix_width(Broodwar->mapWidth() * 32)
, _pix_height(Broodwar->mapHeight() * 32)
, _width(Broodwar->mapWidth() * 4)
, _height(Broodwar->mapHeight() * 4)
, _stormPosMutex(CreateMutex( 
        NULL,                  // default security attributes
        FALSE,                 // initially not owned
        NULL))                  // unnamed mutex
, _lastStormUpdateFrame(0)
{
    if (_stormPosMutex == NULL) 
    {
        Broodwar->printf("CreateMutex error: %d\n", GetLastError());
        return;
    }
    walkability = new bool[_width * _height];             // Walk Tiles resolution
    buildings_wt = new bool[_width * _height];
    buildings_wt_strict = new bool[_width * _height];
    lowResWalkability = new bool[Broodwar->mapWidth() * Broodwar->mapHeight()]; // Build Tiles resolution
    buildings = new bool[Broodwar->mapWidth() * Broodwar->mapHeight()];         // [_width * _height / 16];
    groundDamages = new int[Broodwar->mapWidth() * Broodwar->mapHeight()];
    airDamages = new int[Broodwar->mapWidth() * Broodwar->mapHeight()];
    groundDamagesGrad = new Vec[Broodwar->mapWidth() * Broodwar->mapHeight()];
    airDamagesGrad = new Vec[Broodwar->mapWidth() * Broodwar->mapHeight()];

    // initialization
    for (int x = 0; x < _width; ++x) 
        for (int y = 0; y < _height; ++y) 
        {
            walkability[x + y*_width] = Broodwar->isWalkable(x, y);
            buildings_wt[x + y*_width] = false;
            buildings_wt_strict[x + y*_width] = false;
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
            lowResWalkability[x + y*_width/4] = walkable;
            buildings[x + y*_width/4] = false; // initialized with manual call to onUnitCreate() in onStart()
            groundDamages[x + y*_width/4] = 0;
            groundDamagesGrad[x + y*_width/4] = Vec(0, 0);
            airDamages[x + y*_width/4] = 0;
            airDamagesGrad[x + y*_width/4] = Vec(0, 0);
        }
    }
    _eUnitsFilter = NULL;
}

MapManager::~MapManager()
{
#ifdef __DEBUG_GABRIEL__
    Broodwar->printf("MapManager destructor");
#endif
    delete [] walkability;
    delete [] lowResWalkability;
    delete [] buildings_wt;
    delete [] buildings;
    delete [] groundDamages;
    delete [] airDamages;
    delete [] groundDamagesGrad;
    delete [] airDamagesGrad;
    CloseHandle(_stormPosMutex);
}

void MapManager::setDependencies()
{
    _eUnitsFilter = & EUnitsFilter::Instance();
}

void MapManager::modifyBuildings(Unit* u, bool b)
{
    // TODO Optimize (3 loops are unecessary)
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
    int higherX = (p.x() + tmpMaxRadius) < _width ? p.x() + tmpMaxRadius : _pix_width;
    int lowerY = (p.y() - tmpMaxRadius) > 0 ? p.y() - tmpMaxRadius : 0;
    int higherY = (p.y() + tmpMaxRadius) < _height ? p.y() + tmpMaxRadius : _pix_height;
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
    int higherX = (p.x() + tmpMaxRadius) < _width ? p.x() + tmpMaxRadius : _pix_width;
    int lowerY = (p.y() - tmpMaxRadius) > 0 ? p.y() - tmpMaxRadius : 0;
    int higherY = (p.y() + tmpMaxRadius) < _height ? p.y() + tmpMaxRadius : _pix_height;
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
        updateDamagesGrad(this->groundDamagesGrad, this->groundDamages, p, ut.groundWeapon().minRange(), ut.groundWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft());
    }
    if (ut.airWeapon() != BWAPI::WeaponTypes::None)
    {
        int addRange = additionalRangeAir(ut);
        modifyDamages(this->airDamages, p, ut.airWeapon().minRange(), ut.airWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft(), 
            - ut.airWeapon().damageAmount() * ut.maxAirHits());
        updateDamagesGrad(this->airDamagesGrad, this->airDamages, p, ut.airWeapon().minRange(), ut.airWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft());
    }
}

void MapManager::removeDmgStorm(Position p)
{
    modifyDamages(this->groundDamages, p, 0, 63, -50);
    modifyDamages(this->airDamages, p, 0, 63, -50);
    updateDamagesGrad(this->groundDamagesGrad, this->groundDamages, p, 0, 63);
    updateDamagesGrad(this->airDamagesGrad, this->airDamages, p, 0, 63);
}

void MapManager::addDmg(UnitType ut, Position p)
{ 
    if (ut.groundWeapon() != BWAPI::WeaponTypes::None)
    {
        int addRange = additionalRangeGround(ut);
        modifyDamages(this->groundDamages, p, ut.groundWeapon().minRange(), ut.groundWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft(), 
            ut.groundWeapon().damageAmount() * ut.maxGroundHits());
        updateDamagesGrad(this->groundDamagesGrad, this->groundDamages, p, ut.groundWeapon().minRange(), ut.groundWeapon().maxRange() + addRange);
    }
    if (ut.airWeapon() != BWAPI::WeaponTypes::None)
    {
        int addRange = additionalRangeAir(ut);
        modifyDamages(this->airDamages, p, ut.airWeapon().minRange(), ut.airWeapon().maxRange() + addRange + ut.dimensionRight() + ut.dimensionLeft(), 
            ut.airWeapon().damageAmount() * ut.maxAirHits());
        updateDamagesGrad(this->airDamagesGrad, this->airDamages, p, ut.airWeapon().minRange(), ut.airWeapon().maxRange() + addRange);
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
    else 
        return 0;
}

void MapManager::onUnitCreate(Unit* u)
{
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
        if (u->getType() == UnitTypes::Terran_Vulture_Spider_Mine)  // hack, see.h
            _trackedMines.erase(u);                                 // hack, see.h
    }
    else
    {
        removeAlliedUnit(u);
    }
}

void MapManager::onUnitShow(Unit* u)
{
    addBuilding(u);
    if (u->getPlayer() == Broodwar->self())
        addAlliedUnit(u);
}

void MapManager::onUnitHide(Unit* u)
{
    addBuilding(u);
}

DWORD WINAPI MapManager::StaticLaunchUpdateStormPos(void* obj)
{
    MapManager* This = (MapManager*) obj;
    int buf = This->LaunchUpdateStormPos();
    ExitThread(buf);
    return buf;
}

DWORD MapManager::LaunchUpdateStormPos()
{
    DWORD waitResult = WaitForSingleObject(
        _stormPosMutex,
        100);

    switch (waitResult) 
    {
    case WAIT_OBJECT_0: 
        updateStormPos();
        ReleaseMutex(_stormPosMutex);
        break; 

    case WAIT_ABANDONED:
        ReleaseMutex(_stormPosMutex);
        return -1;
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
            if (eit->second.x() > it->x() - 8 && eit->second.x() < it->x() + 8
                && eit->second.y() > it->y() - 8 && eit->second.y() < it->y() + 8)
                ++tmp;
        }
        for (std::map<BWAPI::Unit*, std::pair<BWAPI::UnitType, BWAPI::Position> >::const_iterator iit = _invisibleUnitsBuf.begin();
            iit != _invisibleUnitsBuf.end(); ++ iit)
        {
            if (iit->second.first != UnitTypes::Protoss_Observer && iit->second.first != UnitTypes::Zerg_Zergling
                && iit->second.second.x() > it->x() - (__STORM_SIZE__ / 2 + 1) && iit->second.second.x() < it->x() + (__STORM_SIZE__ / 2 + 1)
                && iit->second.second.y() > it->y() - (__STORM_SIZE__ / 2 + 1) && iit->second.second.y() < it->y() + (__STORM_SIZE__ / 2 + 1))
                ++tmp;            
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
            std::map<Position, int>::iterator tmp = it;
            ++it;
            stormPos.erase(tmp);
        }
        else
            ++it;
    }
    if (_dontReStorm.count(p))
        _dontReStorm[p] = 0;
    else
        _dontReStorm.insert(std::make_pair<Position, int>(p, 0));
}

void MapManager::onFrame()
{
#ifdef __DEBUG_GABRIEL__
    clock_t start = clock();
#endif
    // update our units' positions
    for (std::map<BWAPI::Unit*, BWAPI::Position>::iterator it = _ourUnits.begin();
        it != _ourUnits.end(); ++it)
    {
        _ourUnits[it->first] = it->first->getPosition();
    }

    // check/update the damage maps. BEWARE: hidden units are not removed in presence of doubt!
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
                addDmgWithoutGrad(it->first->getType(), it->first->getPosition());
                removeDmg(it->first->getType(), _trackedUnits[it->first]);
                _trackedUnits[it->first] = it->first->getPosition();
            }
        }
        else
        {
            if (it->first->exists()
                && it->first->isVisible())
            {
                // add it to MapManager (ourselves)
                addDmg(it->first->getType(), it->first->getPosition());
                _trackedUnits.insert(std::make_pair<Unit*, Position>(it->first, it->first->getPosition()));
            }
        }
        if (it->second.type == UnitTypes::Terran_Vulture_Spider_Mine)                               // hack with _trackedMines, see .h
            _trackedMines.insert(std::make_pair<Unit*, Position>(it->first, it->second.position));  // hack with _trackedMines, see .h
        if (!(it->first->isVisible()))
        {
            _eUnitsFilter->filter(it->first);
        }
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
    std::list<Bullet*> stormsToDelete;
    std::list<Unit*> minesToDelete;
    for (std::map<Bullet*, Position>::iterator it = _trackedStorms.begin();
        it != _trackedStorms.end(); ++it)
    {
        if (!it->first->exists())
        {
            removeDmgStorm(it->second);
            for (std::map<Unit*, Position>::iterator i = _trackedMines.begin();     // hack with _trackedMines, see .h
                i != _trackedMines.end(); ++i)                                         // hack with _trackedMines, see .h
            {
                if (i->second.x() > it->second.x() - (__STORM_SIZE__ / 2 + 1) && i->second.x() < it->second.x() + (__STORM_SIZE__ / 2 + 1)        // hack with _trackedMines, see .h
                    && i->second.y() > it->second.y() - (__STORM_SIZE__ / 2 + 1) && i->second.y() < it->second.y() + (__STORM_SIZE__ / 2 + 1))    // hack with _trackedMines, see .h
                {
                    removeDmg(UnitTypes::Terran_Vulture_Spider_Mine, i->second);    // hack with _trackedMines, see .h
                    _eUnitsFilter->onUnitDestroy(i->first);                         // hack with _trackedMines, see .h
                    //std::map<Unit*, Position>::iterator tmp = i;                    // hack with _trackedMines, see .h
                    //++i;                                                            // hack with _trackedMines, see .h
                    //_trackedMines.erase(tmp);                                       // hack with _trackedMines, see .h
                    minesToDelete.push_back(i->first);
                }
                //else
                    //++i;
            }
            stormsToDelete.push_back(it->first);
        }
    }
    for (std::list<Bullet*>::const_iterator it = stormsToDelete.begin();
        it != stormsToDelete.end(); ++it)
    {
        _trackedStorms.erase(*it);
    }
    for (std::list<Unit*>::const_iterator it = minesToDelete.begin();
        it != minesToDelete.end(); ++it)
    {
        _trackedMines.erase(*it);
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
                it != _dontReStorm.end();)
            {
                if (stormPos.count(it->first))
                    stormPos.erase(it->first);
                _dontReStorm[it->first] = _dontReStorm[it->first] + lastUpdateDiff;
                if (it->second > 72)
                {
                    std::map<Position, int>::iterator tmp = it;
                    ++it;
                    _dontReStorm.erase(tmp);
                }
                else
                    ++it;
            }
            _lastStormUpdateFrame = Broodwar->getFrameCount();

            /// Prepare for the next update of _stormPosBuf thread
            _enemyUnitsPosBuf = HighTemplarUnit::stormableUnits;
            if (_enemyUnitsPosBuf.size())
                Broodwar->printf("got something");
            Broodwar->printf("last error %s", Broodwar->getLastError().toString().c_str());
            if (!_enemyUnitsPosBuf.empty())
            {
                _alliedUnitsPosBuf = _ourUnits;
                _invisibleUnitsBuf = _eUnitsFilter->getInvisibleUnits();
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
                DWORD threadId;
                HANDLE thread = CreateThread( 
                    NULL,                   // default security attributes
                    0,                      // use default stack size  
                    &MapManager::StaticLaunchUpdateStormPos,      // thread function name
                    (void*) this,                   // argument to thread function 
                    0,                      // use default creation flags 
                    &threadId);             // returns the thread identifier 
                if (thread == NULL)
                {
                    Broodwar->printf("(mapmanager) error creating thread");
                }
                CloseHandle(thread);
            }
        }
        ReleaseMutex(_stormPosMutex);
    }

#ifdef __DEBUG_GABRIEL__
    clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;
    if (duration > 0.040) 
        Broodwar->printf("MapManager::onFrame() took: %2.5f seconds\n", duration);
#endif
    this->drawGroundDamagesGrad();
    this->drawGroundDamages();
    this->drawBestStorms();
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

void MapManager::drawBuildings()
{
    for (int x = 0; x < _width; ++x)
        for (int y = 0; y < _height; ++y)
        {
            if (buildings_wt[x + y*_width])
                Broodwar->drawBoxMap(8*x+1, 8*y+1, 8*x+7, 8*y+7, Colors::Orange);
        }
}

void MapManager::drawBuildingsStrict()
{
    for (int x = 0; x < _width; ++x)
        for (int y = 0; y < _height; ++y)
        {
            if (buildings_wt_strict[x + y*_width])
                Broodwar->drawBoxMap(8*x+1, 8*y+1, 8*x+7, 8*y+7, Colors::Orange);
        }
}

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
            if (!lowResWalkability[x + y*Broodwar->mapWidth()])
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