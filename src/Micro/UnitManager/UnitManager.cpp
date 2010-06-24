#include "UnitManager.h"

#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;

/*************** Constructors and destructors ***************/

/*
 *  \brief default constructor
 */
UnitManager::UnitManager()
{
	BaseLocation* startLoc = BWTA::getStartLocation(BWAPI::Broodwar->self());

	BWAPI::Position pos;

	const std::set<Region*>& region = getRegions();
	const std::set<Chokepoint*>& chocke = getChokepoints();
	for( std::set<Region*>::const_iterator itRegion = region.begin(); itRegion != region.end(); itRegion++)
	{
		if( (*itRegion)->getPolygon().isInside(startLoc->getPosition()))
		{
			for( std::set<Chokepoint*>::const_iterator itChocke = chocke.begin(); itChocke != chocke.end(); itChocke++)
			{
				const std::pair<Region*,Region*>& border = (*itChocke)->getRegions();
				if( border.first == *itRegion || border.second == *itRegion )
				{
					pos = (*itChocke)->getCenter();
				}
			}
		}
	}

	UnitGroup* firstGroup = new UnitGroup(pos);
	lGroup_.push_back(firstGroup);

	regions = & Regions::Instance();
}

/*
 *  \brief destructor
 */
UnitManager::~UnitManager()
{
}


/************************* Accessors ************************/

/*
 *  \brief return the name of the class
 *  \return the name of the class
 */
std::string UnitManager::getName() const
{
	return "Unit Manager";
}


/************************** Update **************************/

/*
 *  \brief update the unit manager at each time step
 */
void UnitManager::update()
{
	this->checkNewObjectives();
	this->assignGroupToObj();
}

/*
 *  \brief check for new objectives to create
 */
void UnitManager::checkNewObjectives()
{
	for( std::list<UnitGroup*>::iterator itGroup = lGroup_.begin(); itGroup != lGroup_.end(); ++itGroup)
	{
		if( (*itGroup)->nbUnits() > 15)
		{
			// Get the enemy position
			const Position* enemyLocation = NULL;
			for( map<Region*, RegionData>::iterator itRegions = regions->regionsData.begin(); itRegions != regions->regionsData.end(); itRegions++)
				for( map<Player*, vector<BuildingData> >::iterator itBuildings = itRegions->second.buildings.begin(); itBuildings != itRegions->second.buildings.end(); itBuildings++)
					if( itBuildings->first->isEnemy( Broodwar->self()))
						enemyLocation =	&(itRegions->first->getCenter());

			// Send the group attack the enemy
			(*itGroup)->attackTo( *enemyLocation);
		}
	}
}

/*
 *  \brief assign groups of units to the current objectives to fulfill
 */
void UnitManager::assignGroupToObj()
{
	// check if an objective has not been asigned
	// and if any group of unit can fulfill it
	std::list<Objective*>::iterator it = lObjective_.begin();
	while(it != lObjective_.end())
	{
		if((*it)->status() == OBJ_NOT_ASSIGNED)
		{
			bool found = false;
			std::list<UnitGroup*>::iterator it2 = lGroup_.begin();
			while(!found && it2 != lGroup_.end())
			{
				if((*it2)->canFulFillObj(*it))
				{
					(*it)->assignGroup(*it2);
					(*it)->start();
				}
				else
				{
					it2++;
				}
			}
		}
		it++;
	}
}


/******* Modification of the list of available units ********/

/*
 *  \brief add a new unit
 *  \param unit : the new created unit
 */
void UnitManager::onUnitCreate(BWAPI::Unit* unit)
{
	// check for each incomplete group if it requires the input unit
	bool found = false;
	std::list<UnitGroup*>::iterator it = lGroup_.begin();
	while(!found && it != lGroup_.end())
	{
		if(!(unit->getType().isBuilding()) && !(unit->getType().isWorker()) &&  (*it)->requires(unit))
		{
			(*it)->addUnit(unit);
			BWAPI::Broodwar->printf("A unit has been assigned to a group");
			found = true;
			unit->attackMove((*it)->curPos());
		}
		it++;
	}

	if(!found)
	{
		// should create a new group and add the unit into it
		BWAPI::Broodwar->printf("Unit not managed :-( (not assigned to a group)");
	}
}

/*
 *  \brief add a new unit
 *  \param unit : the new created unit
 */
void UnitManager::onUnitDestroy(BWAPI::Unit* unit)
{
}
