#include <algorithm>
#include "Goal.h"
#include "UnitsGroup.h"



Goal::~Goal()
{
}

Goal::Goal(GoalType t):
status(GS_NOT_STARTED),
formation(pFormation(boost::shared_ptr<Formation>())),
type(t)
{
	

}


Goal::Goal():
status(GS_NOT_STARTED),
formation(pFormation(boost::shared_ptr<Formation>())),
type(GT_UNDEFINED)
{
}

void Goal::achieve(UnitsGroup* ug)
{
	checkAchievement(ug);
	if(this->status!=GS_ACHIEVED){
		//TOIMPROVE
		pSubgoal sub;

		//Select first subgoal not accomplished
		for(std::list<pSubgoal>::iterator p = subgoals.begin(); p != subgoals.end(); p++){
			if((*p)->subgoalCondition()==SC_ONCE && !(*p)->isRealized()){
				sub=(*p);
				
				ug->attackMove(sub->subgoalPosition());//TOCHANGE ACCORDING TO SUBGOAL TYPE
				
				break;
			}
		}


	}

}

void Goal::checkAchievement(UnitsGroup* ug)
{
		bool ach=true;
		for each (pSubgoal p in subgoals){
			
			if(!p->isRealized()){
				ach=false;
				break;
			}
		}
		if (ach==true){
			this->status = GS_ACHIEVED;
			//BWAPI::Broodwar->printf("Goal achieved");
		}
	
}

void Goal::addSubgoal(pSubgoal s){
	this->subgoals.push_back(s);
}

void Goal::setFormation(pFormation f){
formation = f;
}

pFormation Goal::getFormation() const{
	return formation;
}

GoalStatus Goal::getStatus() const{
	return status;
}
void Goal::setStatus(GoalStatus s) {
	status = s;
}

GoalType Goal::getType() const{
return type;

}

