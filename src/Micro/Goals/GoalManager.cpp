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
	if(goal->getType()==GT_SCOUT){
		//Select a worker
		UnitsGroup* ug;	
		
		for(std::map<UnitsGroup *, std::list<pGoal>>::iterator it_ug = this->attributedGoals.begin(); it_ug != attributedGoals.end(); ++it_ug){
			//Check over all the already created unitsGroup which one is near the first subgoal to accomplish
			//TODO
		}

		/*if (ug ==NULL){
			//NO unitsgroup already found, must create a new one
			BWAPI::Position p = goal->firstPosition();
			Unit * unitToTake;
			double minDist=9999;
			double curDist=0;
			for each(Unit* u in Broodwar->getAllUnits()){
				if (u->getPlayer()==Broodwar->self()&&u->getType().isWorker()&& !(u->isConstructing())){
					curDist=p.getDistance(u->getPosition());
					if(curDist<minDist){
						unitToTake = u;
						minDist=curDist;
					}
				}

			}
			//if(unitToTake != NULL){ // TOCHECK (DEF PROG)
				ug = new UnitsGroup();
			
				ug->takeControl(unitToTake);
			//}
		}*/
		if (ug ==NULL){
			for each(Unit* u in Broodwar->getAllUnits()){
				if (u->getPlayer()==Broodwar->self()&&u->getType().isWorker()&& !(u->isConstructing())){
					ug = new UnitsGroup();
					ug->takeControl(u);
					break;
				}
			}
		}

		if (ug != NULL){
			//Check if the unitsGroup is not empty else Segfault ?
			if (ug->getUnits()->size() != 0) {
			//TOCHECK
				ug->addGoal(goal);
				insert(ug, goal);
				microManager->unitsgroups.push_back(ug);
			//	Broodwar->printf("Unit found, goal attributed");
			}else{
				Broodwar->printf("Could not find an appropriate unit for this scout goal");
				Broodwar->printf("Problem...");
			}

		}
	}else if(goal->getType()==GT_ATTACK){

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

	if (p->getType() == GT_SCOUT){
	//	BWAPI::Broodwar->printf("Receive the order to destroy the group");
		//Destroy the units group : remove units
		microManager->promptRemove(ug);
		ug->~UnitsGroup();
	//	if(!this->microManager->remove(ug)){
	//		BWAPI::Broodwar->printf("Could not find the group");
	//	}
		//ug->~UnitsGroup();
	}

}