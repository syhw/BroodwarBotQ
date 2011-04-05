#include <PrecompiledHeader.h>
#include "GoalManager.h"

using namespace BWAPI;
GoalManager::GoalManager()
{
	this->warManager = NULL;
}

GoalManager::~GoalManager()
{
}
void GoalManager::setDependencies()
{
	this->warManager = & WarManager::Instance();
}

void GoalManager::insert(UnitsGroup * ug, pGoal g)
{
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