#include <algorithm>
#include "Goal.h"
#include "UnitsGroup.h"
#include "float.h"

Goal::~Goal()
{
}

Goal::Goal():
status(GS_NOT_STARTED)
{
}

void Goal::achieve(UnitsGroup* ug)
{
	checkAchievement(ug);
			
	if(this->status!=GS_ACHIEVED){
		double test;
		pSubgoal selected;
		double min = DBL_MAX;
		
		for(std::list<pSubgoal>::iterator it = subgoals.begin(); it != subgoals.end(); ++it){
			if (!(*it)->isRealized()){
				test = (*it)->distanceToRealize(ug);
				if ( test >= 0 && test < min ){
					min = test;
					selected = (*it);
				}
			}
		}
		if(min > 0 && min < DBL_MAX){
			selected->tryToRealize(ug);

		}else{
			//TODO
			BWAPI::Broodwar->printf("Tell Louis this situation happened");
		}

	}


}

void Goal::checkAchievement(UnitsGroup* ug)
{
	//All the subgoals are tested because the check function might validate
	//some subgoals
		bool res_and=true;
		bool res_or=false;
		
		for each (pSubgoal p in subgoals){
			
			if(p->getLogic() == SL_AND){
				//AND case 
				if(!p->isRealized()){
					res_and=false;
				}

			} else {
				//OR case
				if(p->isRealized()){
				res_or = true;
				}
			}
		}
			
		if(res_and || res_or){
			this->status= GS_ACHIEVED;
		}
	
}

void Goal::addSubgoal(pSubgoal s){
	this->subgoals.push_back(s);
}

GoalStatus Goal::getStatus() const{
	return status;
}
void Goal::setStatus(GoalStatus s) {
	status = s;
}
