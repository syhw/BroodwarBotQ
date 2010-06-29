#include <MicroManager.h>
#include <Regions.h>
#include <util.h>
#include <UnitsGroup.h>

using std::map;
using std::set;
using std::vector;
using std::list;
using namespace BWAPI;
using namespace BWTA;

MicroManager::MicroManager() 
{
	this->arbitrator = & Arbitrator::Arbitrator<BWAPI::Unit*,double>::Instance();
	this->regions = & Regions::Instance();
}

MicroManager::~MicroManager() 
{
	Broodwar->printf("INOUT MicroManager::~MicroManager()");
}

void MicroManager::update()
{
	if (unitsgroups.empty()) return;

	for (std::list<UnitsGroup*>::iterator it = unitsgroups.begin(); it != unitsgroups.end(); it++)
	{
		UnitsGroup* ug = *it;
		ug->update();

		if (ug->emptyGoals())
		{
		//	if (ug->getLastGoal()->type == GT_ATTACK_BASE)
		//		sendGroupToAttack( ug);

		//	if (ug->getLastGoal()->type == GT_DEFEND_BASE && ug->getNbUnits() > 9)
		//	{
		//		sendGroupToAttack (ug);
				// Create a new defense group.
				UnitsGroup* ug2 = new UnitsGroup();
				unitsgroups.push_back( ug2);
		//		sendGroupToDefense (ug2);
		//	}
		}
	}
}


void MicroManager::onOffer(std::set<BWAPI::Unit*> units)
{
	for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
	{
		if (!(*u)->getType().isWorker() && !(*u)->getType().isBuilding())
		{
			arbitrator->accept(this, *u);
			if (unitsgroups.empty())
			{
				UnitsGroup* ug = new UnitsGroup();
				unitsgroups.push_back( ug);
				sendGroupToDefense (ug);
			}

			unitsgroups.back()->takeControl(*u);
			//Broodwar->printf("New %s added to the micro manager", (*u)->getType().getName().c_str());
		}
		else
		{
			arbitrator->decline(this, *u, 0);
		}
	}
}

void MicroManager::onRevoke(BWAPI::Unit* unit, double bid)
{
	this->onUnitDestroy(unit);
}

std::string MicroManager::getName() const
{
	return "micro Manager";
}

void MicroManager::onUnitCreate(BWAPI::Unit* unit)
{
	if (!unit->getType().isWorker() && unit->getPlayer()==Broodwar->self() && !unit->getType().isBuilding() && unit->getType().canAttack())
		arbitrator->setBid(this, unit, 100);
}

void MicroManager::onUnitDestroy(BWAPI::Unit* unit)
{
	for (std::list<UnitsGroup*>::iterator it = unitsgroups.begin(); it != unitsgroups.end();)
	{
		(*it)->giveUpControl(unit);
		if( (*it)->emptyUnits())
		{
			UnitsGroup* ug = *it;
			it = unitsgroups.erase( it);
			delete ug;
		}
		else
			it++;
	}
}

void MicroManager::display()
{
	for( std::list<UnitsGroup*>::iterator it = unitsgroups.begin(); it != unitsgroups.end(); it++)
		(*it)->display();
}

void MicroManager::sendGroupToAttack( UnitsGroup* ug)
{
	// Get the nearest enemy position
	bool found = false;
	double minDist = 0xFFFF;
	Position nearestEnemyLocation;
	Unit* enemyUnit;
	for (map<Region*, RegionData>::const_iterator itRegions = regions->regionsData.begin(); itRegions != regions->regionsData.end(); itRegions++)
	{
		for (map<Player*, vector<UnitData> >::const_iterator itBuildings = itRegions->second.buildings.begin(); itBuildings != itRegions->second.buildings.end(); itBuildings++)
		{
			if (itBuildings->first->isEnemy(Broodwar->self()))
			{
				for (vector<UnitData>::const_iterator itBD = itBuildings->second.begin(); itBD != itBuildings->second.end(); ++itBD)
				{
					double distance = getGroundDistance( ug->getCenter(), itBD->position);
					if( distance < minDist)
					{
						minDist = distance;
						nearestEnemyLocation = itBD->position;
						enemyUnit = itBD->unit;
						found = true;
					}
				}
			}
		}
	}

	if (!found) return;
	//ug->addGoal(pGoal(new AttackGoal(nearestEnemyLocation, enemyUnit)));
	Broodwar->printf( "Let's fight !!");
}

void MicroManager::sendGroupToDefense( UnitsGroup* ug)
{
	// Go to the nearest choke point.
	BaseLocation* startLoc = BWTA::getStartLocation(BWAPI::Broodwar->self());

	BWAPI::Position chokePoint;

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
					chokePoint = (*itChocke)->getCenter();
				}
			}
		}
	}

	// Send the group defend the base
	//ug->addGoal(pGoal(new DefendGoal(chokePoint)));
}