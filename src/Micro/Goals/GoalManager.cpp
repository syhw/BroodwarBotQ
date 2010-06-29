#include "GoalManager.h"

using namespace BWAPI;
GoalManager::GoalManager(){
	this->scoutManager = & ScoutManager::Instance();
	this->microManager = & MicroManager::Instance();
}

GoalManager::~GoalManager(){
}

void GoalManager::onFrame(){
	if(scoutManager->newGoal()){
		pGoal p = scoutManager->getGoal();
		BWAPI::Broodwar->printf("Ici Goal manager, objectif bien recu...Finding unit");
		findUnit(p);
	}
}

void GoalManager::findUnit(pGoal goal){
	//TODO
	//For the moment take a worker for scout goals


	if(goal->type==SCOUT){
		//Select a worker
		UnitsGroup* ug;	
	
		for each(Unit* u in Broodwar->getAllUnits()){
			if (u->getPlayer()==Broodwar->self()&&u->getType().isWorker()&& !(u->isConstructing())){
			ug = new UnitsGroup();
			ug->takeControl(u);
			break;
			}
		}
	//Check if the unitsGroup is not empty else Segfault ?
		if (ug->getUnits()->size() != 0) {
			goal->status = GS_MOVE_TO;
			ug->addGoal(goal);
			microManager->unitsgroups.push_back(ug);
			Broodwar->printf("Unit found, goal attributed");
		}else{
			Broodwar->printf("Could not find an appropriate unit for this scout goal");
		}
	}


	
}