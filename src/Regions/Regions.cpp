#include <Regions.h>

using std::map;
using std::vector;
using namespace BWAPI;
using namespace BWTA;

/////////////////////////////////////////////////
////////            UNIT DATA             ///////
/////////////////////////////////////////////////

RegionsUnitData::RegionsUnitData(Unit* u)
: unit(u)
, unitType(unit->getType())
, position(unit->getPosition())
, lastSeen(BWAPI::Broodwar->getFrameCount())
{
}

bool RegionsUnitData::operator == ( const RegionsUnitData& ud) const
{
    if (unit == NULL || ud.unit == NULL)
        return false;
    else
        return unit == ud.unit;
}


/////////////////////////////////////////////////
////////          REGIONS DATA            ///////
/////////////////////////////////////////////////

RegionData::RegionData()
: lastSeen(BWAPI::Broodwar->getFrameCount())
, visited(0)
{
}

bool RegionData::isOccupied() const
{
	return !lastSeen && !buildings.empty();
}

bool RegionData::contain(Unit* unit) const
{
	RegionsUnitData test( unit);
	for( map<Player*, vector<RegionsUnitData> >::const_iterator itBuild = buildings.begin(); itBuild != buildings.end(); itBuild++)
		for( vector<RegionsUnitData>::const_iterator itBD = itBuild->second.begin(); itBD != itBuild->second.end(); itBD++)
			if( (*itBD) == test) return true;
	return false;
}

void RegionData::add(Unit* unit)
{
    if (unit->getType().isBuilding())
        buildings[unit->getPlayer()].push_back(RegionsUnitData(unit));
    else 
        units[unit->getPlayer()].push_back(RegionsUnitData(unit));  
    //BWAPI::Broodwar->printf("I just added %s \n", unit->getType().getName());
}


/////////////////////////////////////////////////
////////             REGIONS              ///////
/////////////////////////////////////////////////

Regions::Regions()
: BaseObject("Regions")
{
	timeManager = NULL;
    mapManager = NULL;
}

Regions::~Regions()
{

}

void Regions::setDependencies(){
	this->timeManager = & TimeManager::Instance();
	this->mapManager = & MapManager::Instance();
}

Region* Regions::findRegion(BWAPI::Position p)
{
    const std::set<Region*>& regions = getRegions();
	for( std::set<Region*>::const_iterator itRegion = regions.begin(); itRegion != regions.end(); ++itRegion)
	{
        const BWTA::Polygon& poly = (*itRegion)->getPolygon();
		if (poly.isInside(p))
		    return *itRegion;
    }
    Broodwar->printf("findRegion(%d, %d) returned NULL, something bad happened\n", p.x(), p.y());
    return NULL;
}

void Regions::update()
{	
    //if(!(timeManager->getElapsedTime() % 96))  // todo remplacer par callback
    if (Broodwar->getFrameCount() % 42)
    {
        removeUnits();                         // remove units AND buildings
        addUnits();
    }
}

std::string Regions::getName() const
{
	return "Regions component";
}

void Regions::onUnitCreate(Unit* unit)
{
    addUnit(unit);
}

void Regions::onUnitDestroy(Unit* unit)
{
	removeUnit(unit);
}

void Regions::onUnitShow(Unit* unit)
{
    addUnit(unit);
}

void Regions::onUnitHide(Unit* unit)
{
    addUnit(unit);
}

void Regions::addUnits()
{
    for (map<Region*, RegionData>::iterator itRD = regionsData.begin(); 
        itRD != regionsData.end(); ++itRD)
    {
        // units (if a building is no longer lifted, remove it from units and
        // add it in buildings)
        for (map<Player*, vector<RegionsUnitData> >::iterator itBuild 
            = itRD->second.buildings.begin(); 
            itBuild != itRD->second.buildings.end(); ++itBuild)
        {
            for (vector<RegionsUnitData>::iterator itBD = itBuild->second.begin(); 
                itBD != itBuild->second.end(); ++itBD)
            {
                if (Broodwar->isVisible(itBD->unit->getPosition().x()/32, 
                    itBD->unit->getPosition().y()/32) && !itBD->unit->isLifted())
                    mapManager->onUnitCreate(itBD->unit); // high cost to do that, we should keep track of lifted only
            }
        }
    }
}

void Regions::addUnit(BWAPI::Unit* unit)
{
    if (unit == NULL) 
    {
        Broodwar->printf("Unit is NULL, WTF?\n");
        return;
    }
    if (!unit->exists())
        return;
    if (unit->getPlayer()->isNeutral() || !unit->getPlayer()->isEnemy(Broodwar->self())) return;      
    RegionData& rd = regionsData[findRegion(unit->getPosition())]; // if findRegion returns NULL, something really bad happened
    if (!rd.contain(unit)) {
        rd.add(unit);
    } else {
        if (unit->getType().isBuilding())
            rd.buildings[unit->getPlayer()][rd.buildings.size() - 1].lastSeen = BWAPI::Broodwar->getFrameCount();
        else
            rd.units[unit->getPlayer()][rd.units.size() - 1].lastSeen = BWAPI::Broodwar->getFrameCount();
    }
    rd.lastSeen = BWAPI::Broodwar->getFrameCount();
}

void Regions::removeUnits()
{
    // Remove destroyed builinds or killed/disapeared units
    for (map<Region*, RegionData>::iterator itRD = regionsData.begin(); itRD != regionsData.end(); ++itRD)
	{
        // buildings
		for (map<Player*, vector<RegionsUnitData> >::iterator itBuild = itRD->second.buildings.begin(); itBuild != itRD->second.buildings.end(); )
		{
			for (vector<RegionsUnitData>::iterator itBD = itBuild->second.begin(); itBD != itBuild->second.end(); )
			{
				if (Broodwar->isVisible(itBD->position.x()/32, itBD->position.y()/32))
                {
                    if (!itBD->unit->exists()) 
                    {
                        mapManager->onUnitDestroy(itBD->unit);
                        itBD = itBuild->second.erase(itBD);
                    } 
                    else if (itBD->unit->isLifted())
                    { 
                        mapManager->onUnitDestroy(itBD->unit); 
                        ++itBD;
                    }
                    else
                        ++itBD;
                }
				else
					++itBD;
			}
			if (itBuild->second.empty())
				itBuild = itRD->second.buildings.erase(itBuild);
			else
				++itBuild;
		}
        // units
        for (map<Player*, vector<RegionsUnitData> >::iterator itUnit = itRD->second.units.begin(); itUnit != itRD->second.units.end(); )
		{
			for (vector<RegionsUnitData>::iterator itUD = itUnit->second.begin(); itUD != itUnit->second.end(); )
			{
				if (Broodwar->isVisible(itUD->position.x()/32, itUD->position.y()/32) && !itUD->unit->exists())
					itUD = itUnit->second.erase(itUD);
				else
					++itUD;
			}
			if (itUnit->second.empty())
				itUnit = itRD->second.units.erase(itUnit);
			else
				++itUnit;
		}
    }
}

void Regions::removeUnit(Unit* unit)
{
    for (map<Region*, RegionData>::iterator itRD = regionsData.begin(); itRD != regionsData.end(); ++itRD)
	    if (unit->getType().isBuilding())		
			for (map<Player*, vector<RegionsUnitData> >::iterator itBuild = itRD->second.buildings.begin(); itBuild != itRD->second.buildings.end(); ++itBuild)
				for (vector<RegionsUnitData>::iterator itBD = itBuild->second.begin(); itBD != itBuild->second.end(); )
					if (itBD->unit == unit)
						itBD = itBuild->second.erase(itBD);
					else
						++itBD;
        else
            for (map<Player*, vector<RegionsUnitData> >::iterator itUnit = itRD->second.units.begin(); itUnit != itRD->second.units.end(); ++itUnit)
                for (vector<RegionsUnitData>::iterator itUD = itUnit->second.begin(); itUD != itUnit->second.end(); )
                    if (itUD->unit == unit)
                        itUD = itUnit->second.erase(itUD);
                    else
                        ++itUD;
}

void Regions::display() const
{
	for (map<Region*, RegionData>::const_iterator itRD = regionsData.begin(); itRD != regionsData.end(); ++itRD)
		for (map<Player*, vector<RegionsUnitData> >::const_iterator itBuild = itRD->second.buildings.begin(); itBuild != itRD->second.buildings.end(); ++itBuild)
			for (vector<RegionsUnitData>::const_iterator itBD = itBuild->second.begin(); itBD != itBuild->second.end(); ++itBD) 
				Broodwar->drawBox( CoordinateType::Map,
				itBD->position.x() - itBD->unitType.dimensionLeft(),
				itBD->position.y() - itBD->unitType.dimensionUp(),
				itBD->position.x() + itBD->unitType.dimensionRight(),
				itBD->position.y() + itBD->unitType.dimensionDown(),
				Colors::Red);
}


bool Regions::enemyFound() const
{
	for(std::map<BWTA::Region*, RegionData>::const_iterator it = this->regionsData.begin(); it != regionsData.end(); ++it){
	
		for(std::map<Player*, std::vector<RegionsUnitData> >::const_iterator it_b = it->second.buildings.begin(); it_b != it->second.buildings.end(); ++it_b){
			if( it_b->first->isEnemy(BWAPI::Broodwar->self()) && !it_b->second.empty())
				return true;
		}
	}
	return false;

}

BWTA::Region* Regions::whereIsEnemy() {
	
	for(std::map<BWTA::Region*, RegionData>::const_iterator it = this->regionsData.begin(); it != regionsData.end(); ++it){
		for(std::map<Player*, std::vector<RegionsUnitData> >::const_iterator it_b = it->second.buildings.begin(); it_b != it->second.buildings.end(); ++it_b){
			if( it_b->first->isEnemy(BWAPI::Broodwar->self()) && !it_b->second.empty())
				return it->first;
		}
	}
	return NULL;
}

#ifdef BW_QT_DEBUG
QWidget* Regions::createWidget(QWidget* parent) const
{
	return new QLabel(QString("createWidget and refreshWidget undefined for this component."), parent);
}

void Regions::refreshWidget(QWidget* widget) const
{
// TODO update your widget after having defined it in the previous method :)
}
#endif