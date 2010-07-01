#include "GoalManager.h"

using namespace BWAPI;
GoalManager::GoalManager(){
	this->microManager = & MicroManager::Instance();
}

GoalManager::~GoalManager(){
}

void GoalManager::newGoal(pGoal p){
	findUnitsGroup(p);
}

void GoalManager::findUnitsGroup(pGoal goal){
	//TODO
	//For the moment take a worker for scout goals
	//And create the units group


	if(goal->getType()==SCOUT){
		//Select a worker
		UnitsGroup* ug;	
		
		for each(Unit* u in Broodwar->getAllUnits()){
			if (u->getPlayer()==Broodwar->self()&&u->getType().isWorker()&& !(u->isConstructing())){
			ug = new UnitsGroup();
			ug->takeControl(u);
			break;
			}
		}
		if (ug != NULL){
			//Check if the unitsGroup is not empty else Segfault ?
			if (ug->getUnits()->size() != 0) {
			//TOCHECK
				ug->addGoal(goal);
				insert(ug, goal);
				microManager->unitsgroups.push_back(ug);
				Broodwar->printf("Unit found, goal attributed");
			}else{
				Broodwar->printf("Could not find an appropriate unit for this scout goal");
			}

		}
	}

	
}





void GoalManager::insert(UnitsGroup * ug, pGoal g){
	//Search if the ug is already in
	std::map<UnitsGroup *, std::list<pGoal> >::iterator it = this->attributedGoals.find(ug);
	if (it != attributedGoals.end()) {
		//The ug is already stored
		it->second.push_back(g);

	} else {
		//Must create the pair ug,list
		std::list<pGoal> l;
		l.push_back(g);
		this->attributedGoals.insert(std::make_pair(ug, l));//TODO
	}

}

bool GoalManager::remove(UnitsGroup * ug, pGoal g){
	//Search if the ug is in
	std::map<UnitsGroup *, std::list<pGoal> >::iterator it = this->attributedGoals.find(ug);
	if(it != attributedGoals.end()){
		//ug exists search the goal in the list
		for (std::list<pGoal>::iterator it_l = it->second.begin(); it_l != it->second.end(); it_l++){
			if((*it_l)==g){
				it->second.erase(it_l);
				return true;
			}
		}
	}
	return false;
}

bool GoalManager::clean(UnitsGroup * ug){
	std::map<UnitsGroup *, std::list<pGoal> >::iterator it = this->attributedGoals.find(ug);
	if(it != attributedGoals.end()){
		attributedGoals.erase(it);
		return true;
	}
	return false;
}

void GoalManager::goalDone(UnitsGroup * ug, pGoal p){

	if (p->getType() == SCOUT){
		BWAPI::Broodwar->printf("Receive the order to destroy the group");
		//Destroy the units group : remove units
		microManager->promptRemove(ug);
		ug->~UnitsGroup();
	//	if(!this->microManager->remove(ug)){
	//		BWAPI::Broodwar->printf("Could not find the group");
	//	}
		//ug->~UnitsGroup();
	}

}