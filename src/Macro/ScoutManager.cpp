#include <BWTA.h>
#include <ScoutManager.h>
#include "Defines.h"

using namespace BWAPI;
ScoutManager::ScoutManager( )
{
	regions = & Regions::Instance();
	microManager = & MicroManager::Instance();
	to_remove = false;
}

ScoutManager::~ScoutManager( )
{
}

void ScoutManager::update()
{

	if(regions->enemyFound() && to_remove == false){
		to_remove = true;
		//enemy found, must explore his base
		
	}
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
	if(BWAPI::Broodwar->self()->supplyUsed() == 10 && unit->getType().isWorker() && !regions->enemyFound()){
#ifdef __DEBUG_LOUIS__
		BWAPI::Broodwar->printf("gotta find the enemy, ScoutManager created the objective");
#endif
		findEnemy();
		//exploreRegion( (*BWTA::getRegions().begin()));
	}
}


void ScoutManager::findEnemy(){

	//Create a new scoutGoal 
	pGoal goal = pGoal(new ScoutGoal());
	pSubgoal sb;

	//Scout the different possible bases
	BWTA::BaseLocation * myStartLocation = BWTA::getStartLocation(BWAPI::Broodwar->self());
	std::set<BWTA::BaseLocation*> path = BWTA::getStartLocations();
		
	for(std::set<BWTA::BaseLocation*>::iterator p=path.begin();p!=path.end();p++){
		if( (*p) != myStartLocation){
		sb=pSubgoal(new SeeSubgoal(SL_AND, (*p)->getPosition()));
		goal->addSubgoal(sb);
		}
	}
	sb=pSubgoal(new FindSubgoal(SL_OR));
	goal->addSubgoal(sb);

	UnitsGroup* ug = this->findUnitsGroup(goal);
	//Check if the unitsGroup is not empty else Segfault ?
	if (ug->getUnits()->size() != 0) {
	//TOCHECK
		ug->addGoal(goal);
		microManager->unitsgroups.push_back(ug);
		//	Broodwar->printf("Unit found, goal attributed");
	}else{
		Broodwar->printf("Could not find an appropriate unit for this scout goal");
		Broodwar->printf("Problem...");
	}
}


void ScoutManager::onUnitShow(BWAPI::Unit* unit){

}


UnitsGroup* ScoutManager::findUnitsGroup(pGoal goal){

		//Find the appropriate UnitsGroup
	
	//Select a worker
	UnitsGroup* ug;	
	double minDist=999999;
	double curDist=0;
	
		//Check over all the already created unitsGroup which one is near the first subgoal to accomplish
		//TODO

		
	//NO unitsgroup already found, must create a new one
	ug = new UnitsGroup();	
	for each(Unit* u in Broodwar->getAllUnits()){
		if (u->getPlayer()==Broodwar->self()&&u->getType().isWorker()&& !(u->isConstructing())){
			ug->takeControl(u);
			break;
		}
	}
	return ug;
}