#include <WarManager.h>
#include <Regions.h>
#include <util.h>
#include <UnitsGroup.h>
#include "DefendChokeGoal.h"
#include "BorderManager.h"


using std::map;
using std::set;
using std::vector;
using std::list;
using namespace BWAPI;
using namespace BWTA;

WarManager::WarManager() 
: BaseObject("WarManager")
{
	this->defgroup = new UnitsGroup();

	this->arbitrator = NULL;
	this->regions = NULL;
}

WarManager::~WarManager() 
{
	//Broodwar->printf("INOUT WarManager::~WarManager()");
}

void WarManager::setDependencies(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arb, Regions * reg){
	this->arbitrator = arb;
	this->regions = reg;

}

void WarManager::onStart(){
	this->sendGroupToDefense(defgroup);

}
void WarManager::update()
{
	//Suppress the list prompted to suppress 
	for each (UnitsGroup * ug in this->promptedRemove){
		this->remove(ug);
		ug->idle();	//Set target of units to their position so that they are now idling
		ug->~UnitsGroup();
	}
	promptedRemove.clear();


	//Set bid on appropriate units (not workers and not building)
	std::set<BWAPI::Unit *> myUnits = BWAPI::Broodwar->self()->getUnits();

	for(std::set<BWAPI::Unit *>::iterator it = myUnits.begin(); it != myUnits.end(); ++it){
		if( !(*it)->getType().isBuilding() && !(*it)->getType().isWorker() ){
			this->arbitrator->setBid(this,(*it),40);
		}
	}


	//Update unitsgroup
	defgroup->update();
	if (unitsgroups.empty()) return;
	UnitsGroup* ug;
	for (std::list<UnitsGroup*>::iterator it = unitsgroups.begin(); it != unitsgroups.end(); it++)
	{
		 ug = *it;
		 ug->update();
	}

    sout << "LOL" << sendl; 
    serr << "LOL" << sendl;
}


void WarManager::onOffer(std::set<BWAPI::Unit*> units)
{ 
	
	for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
	{
		if (!(*u)->getType().isWorker() && !(*u)->getType().isBuilding())
		{
			arbitrator->accept(this, *u);
			
			defgroup->takeControl(*u);
			//Broodwar->printf("New %s added to the micro manager", (*u)->getType().getName().c_str());
		}
		else
		{
			arbitrator->decline(this, *u, 0);
		}
	}
}

void WarManager::onRevoke(BWAPI::Unit* unit, double bid)
{
	this->onUnitDestroy(unit);
}

std::string WarManager::getName() const
{
	return "Micro Manager";
}

void WarManager::onUnitCreate(BWAPI::Unit* unit)
{
	/*
	if (!unit->getType().isWorker() && unit->getPlayer()==Broodwar->self() && !unit->getType().isBuilding() && unit->getType().canAttack())
		arbitrator->setBid(this, unit, 100);
	*/
}

void WarManager::onUnitDestroy(BWAPI::Unit* unit)
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

void WarManager::display()
{
	for( std::list<UnitsGroup*>::iterator it = unitsgroups.begin(); it != unitsgroups.end(); it++)
		(*it)->display();
}

void WarManager::sendGroupToAttack( UnitsGroup* ug)
{
	// Get the nearest enemy position
	bool found = false;
	double minDist = 0xFFFF;
	Position nearestEnemyLocation;
	Unit* enemyUnit;
	for (map<Region*, RegionData>::const_iterator itRegions = regions->regionsData.begin(); itRegions != regions->regionsData.end(); itRegions++)
	{
		for (map<Player*, vector<RegionsUnitData> >::const_iterator itBuildings = itRegions->second.buildings.begin(); itBuildings != itRegions->second.buildings.end(); itBuildings++)
		{
			if (itBuildings->first->isEnemy(Broodwar->self()))
			{
				for (vector<RegionsUnitData>::const_iterator itBD = itBuildings->second.begin(); itBD != itBuildings->second.end(); ++itBD)
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
	//	ug->addGoal(pGoal(new AttackGoal(nearestEnemyLocation)));
	//Broodwar->printf( "Let's fight !!");
}

void WarManager::sendGroupToDefense( UnitsGroup* ug)
{
	/*
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
	}*/

	// Send the group defend the base
	pGoal g = pGoal(new DefendChokeGoal(ug,(*BorderManager::Instance().getMyBorder().begin())));
	ug->addGoal(g);
}

void WarManager::checkDefense(){
	//Run through all our bases. Set defense in the main OR in the natural expo, and some defenses in all bases

}


bool WarManager::remove(UnitsGroup* u){
	for(std::list<UnitsGroup *>::iterator it = unitsgroups.begin(); it != unitsgroups.end(); it ++){
		if( (*it) == u){
			unitsgroups.erase(it);
			return true;
		}
	}
	return false;
}

void WarManager::promptRemove(UnitsGroup* ug){
	this->promptedRemove.push_back(ug);
}

std::set<Unit*> WarManager::getEnemies()
{
    std::set<BWAPI::Player*>::iterator iter = Broodwar->getPlayers().begin();
    for(;iter != Broodwar->getPlayers().end() && !(*iter)->isEnemy(Broodwar->self());iter++);
    return (*iter)->getUnits();
}

#ifdef BW_QT_DEBUG
QWidget* WarManager::createWidget(QWidget* parent) const
{
	return new QLabel(QString("createWidget and refreshWidget undefined for this component."), parent);
}

void WarManager::refreshWidget(QWidget* widget) const
{
// TODO update your widget after having defined it in the previous method :)
}
#endif
