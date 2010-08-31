#include <WarManager.h>
#include <Regions.h>
#include <util.h>
#include <UnitsGroup.h>
#include "DefendBaseGroundGoal.h"
#include "BorderManager.h"
#include "AttackGoal.h"


using std::map;
using std::set;
using std::vector;
using std::list;
using namespace BWAPI;
using namespace BWTA;

WarManager::WarManager() 
: BaseObject("WarManager")
{
	arbitrator = NULL;
	regions = NULL;
    unitsGroups.push_front(new UnitsGroup()); // to garbage collect idle units
    informationManager = & InformationManager::Instance();
    home = BWTA::getStartLocation(Broodwar->self());
}

WarManager::~WarManager() 
{
	//Broodwar->printf("INOUT WarManager::~WarManager()");
}

void WarManager::setDependencies()
{
	this->arbitrator = & Arbitrator::Arbitrator<BWAPI::Unit*,double>::Instance();
	this->regions = & Regions::Instance();
}

void WarManager::onStart(){


}
void WarManager::update()
{

	//Suppress the list prompted to suppress 
	for(std::list<UnitsGroup *>::iterator ug = this->promptedRemove.begin() ; ug != this->promptedRemove.end(); ++ug){
		this->remove(*ug);
		(*ug)->idle();	//Set target of units to their position so that they are now idling
		delete (*ug); // @merge
		// @merge ug->~UnitsGroup();
	}
	promptedRemove.clear();


	//Set bid on appropriate units (not workers and not building)
	std::set<BWAPI::Unit *> myUnits = BWAPI::Broodwar->self()->getUnits();

	for(std::set<BWAPI::Unit *>::iterator it = myUnits.begin(); it != myUnits.end(); ++it){
		if( !(*it)->getType().isBuilding() && !(*it)->getType().isWorker() ){
			this->arbitrator->setBid(this,(*it),20);
		}
	}

	//Update unitsgroup;
	if (unitsGroups.empty()) return;
	for (std::list<UnitsGroup*>::iterator it = unitsGroups.begin(); it != unitsGroups.end(); it++)
	{
        if ((*it)->size() > 2 && (*it)->emptyGoals())
            sendGroupToAttack(*it);
	        (*it)->update();
	}
}


void WarManager::onOffer(std::set<BWAPI::Unit*> units)
{ 
	
	for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
	{
		if (!(*u)->getType().isWorker() && !(*u)->getType().isBuilding())
		{
			arbitrator->accept(this, *u);
            if (unitsGroups.empty()) // because each UnitsGroup gets destroyed when it becomes empty
            {
                unitsGroups.push_back(new UnitsGroup());
            }
            unitsGroups.front()->takeControl(*u);
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
	// @merge this->onUnitDestroy(unit);
}

std::string WarManager::getName() const
{
	return "War Manager";
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
	for (std::list<UnitsGroup*>::iterator it = unitsGroups.begin(); it != unitsGroups.end(); ++it)
	{
		(*it)->giveUpControl(unit);
		if( (*it)->emptyUnits() )
		{
			this->promptRemove(*it);
		}
	}
}

void WarManager::display()
{
	for( std::list<UnitsGroup*>::iterator it = unitsGroups.begin(); it != unitsGroups.end(); it++)
		(*it)->display();
}

void WarManager::sendGroupToAttack( UnitsGroup* ug)
{
	if(Regions::Instance().enemyFound()){
		ug->addGoal(pGoal(new AttackGoal(ug, Regions::Instance().whereIsEnemy()->getCenter())));
	}

	/*
    Broodwar->printf("sending a group to attack");
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
					double distance = getGroundDistance(ug->getCenter(), itBD->position);
					if (distance < minDist)
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

    if (found)
        return;

    // works only for 1v1 (2 players) maps: 
    for (std::set<TilePosition>::const_iterator it = Broodwar->getStartLocations().begin();
        it != Broodwar->getStartLocations().end(); ++it)
    {
        if (*it == home->getTilePosition())
            continue;
        double distance = getGroundDistance(ug->getCenter(), *it);
        if (distance < minDist)
        {
            minDist = distance;
            nearestEnemyLocation = *it;
            found = true;
        }
    }
    ug->addGoal(pGoal(new AttackGoal(ug, nearestEnemyLocation)));

	//Broodwar->printf( "Let's fight !!");*/
}

void WarManager::sendGroupToDefense( UnitsGroup* ug)
{

}


bool WarManager::remove(UnitsGroup* u){
	for(std::list<UnitsGroup *>::iterator it = unitsGroups.begin(); it != unitsGroups.end(); it ++){
		if( (*it) == u){
			unitsGroups.erase(it);
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

