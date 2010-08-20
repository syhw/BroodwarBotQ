#include <BWTA.h>
#include <ScoutManager.h>
#include "Defines.h"

using namespace BWAPI;
ScoutManager::ScoutManager( )
: BaseObject("ScoutManager")
{
	regions = NULL;
	warManager = NULL;
	to_remove = false;
}

ScoutManager::~ScoutManager( )
{
}

void ScoutManager::setDependencies(){
	this->regions = & Regions::Instance();
	this->warManager = & WarManager::Instance();
	this->arbitrator = &  Arbitrator::Arbitrator<BWAPI::Unit*,double>::Instance();
}

void ScoutManager::update()
{
	//if(regions->enemyFound()){
	//	findUnitsGroup(pGoal(new ExploreGoal(regions->whereIsEnemy())));
	//}
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

void ScoutManager::onUnitCreate(BWAPI::Unit* unit){
	//if(BWAPI::Broodwar->self()->supplyUsed() == 10 && unit->getType().isWorker() && !regions->enemyFound()){
	//	findEnemy();
		//exploreRegion( (*BWTA::getRegions().begin()));
	//}
}

void ScoutManager::onOffer(std::set<BWAPI::Unit*> units){

	std::vector<pGoal> goalsDone;
	BWAPI::Broodwar->printf("call to onoffer");
	std::set<BWAPI::Unit*> remainingUnits = units;//Does it copy the set ?

	//find the best unit for each goal
	//Obs are always the best
	BWAPI::Unit * bestUnit;
	int dist= 0; 

	for(std::list<pGoal>::iterator goals = this->awaitingGoals.begin(); goals != this->awaitingGoals.end(); ++goals){
		
		dist = 0;
		bestUnit = NULL;

		for(std::set<BWAPI::Unit *>::iterator units = remainingUnits.begin(); units != remainingUnits.end(); ++units ){
			
			if(bestUnit == NULL){
				bestUnit = (*units);
			}

			if((*units)->getType() == BWAPI::UnitTypes::Protoss_Observer){
				bestUnit = (*units);
				break;
			}
		}

	this->arbitrator->accept(this, bestUnit, 90);
	UnitsGroup * ug = new UnitsGroup();
	ug->takeControl(bestUnit);
	remainingUnits.erase(bestUnit);
	ug->addGoal((*goals));
	(*goals)->setUnitsGroup(ug);
	warManager->unitsgroups.push_back(ug);
	//ug->switchMode(MODE_SCOUT);
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

void ScoutManager::findEnemy(){

	this->awaitingGoals.push_back(pGoal(new FindEnemyGoal()));
	//Set bid on all units that can scout for this goal

	for(std::set<BWAPI::Unit *>::const_iterator it = BWAPI::Broodwar->self()->getUnits().begin(); it != BWAPI::Broodwar->self()->getUnits().end(); ++it){
		
		if( (*it)->getType().isWorker() || (*it)->getType() == BWAPI::UnitTypes::Protoss_Observer ){
			this->arbitrator->setBid(this, (*it),90);
		}
	}

	//findUnitsGroup(goal);
}


void ScoutManager::onUnitShow(BWAPI::Unit* unit){

}


UnitsGroup* ScoutManager::findUnitsGroup(pGoal goal){

}


#ifdef BW_QT_DEBUG
QWidget* ScoutManager::createWidget(QWidget* parent) const
{
	return new QLabel(QString("createWidget and refreshWidget undefined for this component."), parent);
}

void ScoutManager::refreshWidget(QWidget* widget) const
{
// TODO update your widget after having defined it in the previous method :)
}
#endif

