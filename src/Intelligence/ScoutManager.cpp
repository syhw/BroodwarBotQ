#include <BWTA.h>
#include "Defines.h"
#include "ScoutManager.h"
#include "MacroManager.h"

using namespace BWAPI;

ScoutManager::ScoutManager()
: exploringEnemy(false)
, enemyFound(false)
{
	warManager = NULL;
}

ScoutManager::~ScoutManager()
{
}

void ScoutManager::setDependencies()
{
	this->warManager = & WarManager::Instance();
	this->arbitrator = static_cast< Arbitrator::Arbitrator<BWAPI::Unit*,double>* >(& Arbitrator::Arbitrator<BWAPI::Unit*,double>::Instance()) ;
    //// HACK for 2 players map:
    for (std::set<TilePosition>::const_iterator it = Broodwar->getStartLocations().begin();
        it != Broodwar->getStartLocations().end(); ++it)
    {
        if (*it != Broodwar->self()->getStartLocation())
            this->enemyStartLocation = *it;
    }
    //enemyFound = true;
}

void ScoutManager::update()
{
    if (enemyFound && !exploringEnemy) 
    {
		exploringEnemy = true;
		this->awaitingGoals.push_back(pGoal(new ExploreGoal(BWTA::getRegion(enemyStartLocation))));
	}

	std::set<UnitsGroup *> toTrash;
	//Free units that need to be freed and add goals to other
	for(std::list<UnitsGroup *>::iterator it = this->myUnitsGroups.begin(); it != this->myUnitsGroups.end(); ++it ){
		
		if((*it)->isWaiting())
        {
			//If no new goal available release the ug : 
			if(this->awaitingGoals.size() <= 0){
				for(std::vector<pBayesianUnit>::iterator u = (*it)->units.begin(); u != (*it)->units.end(); ++u){
					this->arbitrator->removeBid(this, (*u)->unit);
				}
				this->warManager->promptRemove(*it);
				toTrash.insert(*it);
			} else {
			//New goal available : assign it to the ug
				(*it)->addGoal(this->awaitingGoals.front());
				this->awaitingGoals.pop_front();
			}
		}
	}

	if(this->awaitingGoals.size() > 0)
    {
		//ask units :
		for(std::set<BWAPI::Unit *>::const_iterator it = BWAPI::Broodwar->self()->getUnits().begin(); it != BWAPI::Broodwar->self()->getUnits().end(); ++it){
			if ((*it)->getType().isWorker() || (*it)->getType() == BWAPI::UnitTypes::Protoss_Observer)
            {
				this->arbitrator->setBid(this, (*it),92);
			}
		}
	}
	for(std::set<UnitsGroup *>::iterator trash = toTrash.begin(); trash != toTrash.end(); ++trash){
		this->myUnitsGroups.remove(*trash);
	}
	toTrash.empty();
}

std::string ScoutManager::getName() const
{
	return "Scout Manager";
}

void ScoutManager::scoutAllEnemies()
{
	// TODO
}

void ScoutManager::counterWall()
{
	// TODO
	// se placer ou construire a l'endroit du wall
	// pour photon ensuite ?
}

void ScoutManager::counterBuild()
{
	// TODO
	// En placant le drone aux endroits de construction.
	// Si terran, attaquer le scv qui construit
}

void ScoutManager::harassWorkers()
{
	// TODO
	// hit and run dans les drones aux min
}

void ScoutManager::checkEmptyXP()
{
	//TODO
}



////////////////////////////NEW SECTION

void ScoutManager::onUnitCreate(BWAPI::Unit* unit)
{
}

void ScoutManager::onOffer(std::set<BWAPI::Unit*> units)
{
	std::vector<pGoal> goalsDone;
	std::set<BWAPI::Unit*> remainingUnits = units;

	//Else grab a new unit
	//Obs are always the best
	BWAPI::Unit * bestUnit = NULL;
	int dist= 999999999;
	int test = 0;
	UnitsGroup * giveMeTheGoal = NULL;

	for (std::list<pGoal>::iterator goals = this->awaitingGoals.begin(); goals != this->awaitingGoals.end(); ++goals)
    {
	//find the best unit for each goal
		dist = 99999999;
		bestUnit = NULL;

		for (std::set<BWAPI::Unit *>::iterator units = remainingUnits.begin(); units != remainingUnits.end(); ++units)
        {
					
			if (bestUnit == NULL)
            {
				bestUnit = (*units);
			}
			test = (*goals)->estimateDistance((*units)->getPosition());
			if (test < dist)
            {
				bestUnit = (*units);
				dist = test;
			}
			if ((*units)->getType() == BWAPI::UnitTypes::Protoss_Observer)
            {
				bestUnit = (*units);
				break;
			}
		}
		this->arbitrator->accept(this, bestUnit, 92);
		giveMeTheGoal = new UnitsGroup();
		this->myUnitsGroups.push_back(giveMeTheGoal);
		giveMeTheGoal->takeControl(bestUnit);
        bestUnit->move(Position(Broodwar->mapWidth()*16, Broodwar->mapHeight()*16));
		remainingUnits.erase(bestUnit);
	
		//We have a unitsGroup
		(*goals)->setUnitsGroup(giveMeTheGoal);
		giveMeTheGoal->addGoal((*goals));
	
		warManager->unitsGroups.push_back(giveMeTheGoal);
		giveMeTheGoal->switchMode(MODE_SCOUT);
		goalsDone.push_back((*goals));
	}

	for(std::set<BWAPI::Unit *>::iterator it = remainingUnits.begin(); it != remainingUnits.end(); ++it){
		this->arbitrator->decline(this,(*it), 0);
	}

	for(std::vector<pGoal>::const_iterator it = goalsDone.begin(); it != goalsDone.end(); ++it ){
		this->awaitingGoals.remove(*it);
	}

}


void ScoutManager::onRevoke(BWAPI::Unit* unit, double bid){

}

void ScoutManager::findEnemy()
{
	this->awaitingGoals.push_back(pGoal(new FindEnemyGoal()));
	//Set bid on all units that can scout for this goal

	for (std::set<BWAPI::Unit *>::const_iterator it = BWAPI::Broodwar->self()->getUnits().begin(); it != BWAPI::Broodwar->self()->getUnits().end(); ++it)
    {	
		if ((*it)->getType().isWorker() || (*it)->getType() == BWAPI::UnitTypes::Protoss_Observer)
        {
			this->arbitrator->setBid(this, (*it),92);
		}
	}
}


void ScoutManager::onUnitShow(BWAPI::Unit* unit)
{
    if (unit->getPlayer() != Broodwar->enemy())
        return;
    if (unit->getType() == UnitTypes::Protoss_Nexus 
        || unit->getType() == UnitTypes::Terran_Command_Center 
        || unit->getType() == UnitTypes::Zerg_Hatchery
        || unit->getType() == UnitTypes::Zerg_Lair
        || unit->getType() == UnitTypes::Zerg_Hive)
    {
        if (!enemyFound)
        {
            for (std::set<BWTA::BaseLocation*>::const_iterator it = BWTA::getStartLocations().begin(); // fuck BaseLocation that we can't build from a (Tile)Position
                it != BWTA::getStartLocations().end(); ++it)
            {
                if ((*it)->getTilePosition() == unit->getTilePosition())
                {
                    enemyFound = true;
                    enemyStartLocation = unit->getTilePosition();
                    return;
                }
            }
            if (Broodwar->getFrameCount() > 4320) // 4 minutes
            {
                enemyFound = true;
                enemyStartLocation = unit->getTilePosition();
            }
        } 
        else
        {
            enemyExpandLocations.insert(unit->getTilePosition());
        }
    }
}

void ScoutManager::onUnitDestroy(BWAPI::Unit* unit)
{
	//Find if a ug is concerned
	for(std::list<UnitsGroup *>::iterator it = myUnitsGroups.begin(); it != myUnitsGroups.end(); ++it)
    {
		if ((*it)->emptyUnits())
        {
			this->myUnitsGroups.remove((*it));
			break;
		}
    }
    if (unit->getPlayer() != Broodwar->enemy())
        return;
    if (unit->getType() == UnitTypes::Protoss_Nexus 
        || unit->getType() == UnitTypes::Terran_Command_Center 
        || unit->getType() == UnitTypes::Zerg_Hatchery
        || unit->getType() == UnitTypes::Zerg_Lair
        || unit->getType() == UnitTypes::Zerg_Hive)
    {
        enemyExpandLocations.erase(unit->getTilePosition());
    }
}





