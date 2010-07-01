#include <algorithm>
#include "Goal.h"
#include "UnitsGroup.h"



Goal::~Goal()
{
}

Goal::Goal(GoalType t):
type(t),
status(GS_NOT_STARTED),
purpose(GP_UNDEFINED),
formation(pFormation(boost::shared_ptr<Formation>()))
{
	scoutManager = & ScoutManager::Instance();
}

Goal::Goal(GoalType t, GoalPurpose p):
type(t),
status(GS_NOT_STARTED),
purpose(p),
formation(pFormation(boost::shared_ptr<Formation>()))
{
	scoutManager = & ScoutManager::Instance();
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
				if(this->getType()==SCOUT)
					ug->move(sub->subgoalPosition());
				
				break;
			}
		}


	}

}

void Goal::checkAchievement(UnitsGroup* ug)
{
	if (this->purpose == GP_FINDENEMY){
		if(scoutManager->enemyFound())
			this->status = GS_ACHIEVED;

	} else {
		bool ach=true;
		for each (pSubgoal p in subgoals){
			
			if(!p->isRealized()){
				ach=false;
				break;
			}
		}
		if (ach==true){
			this->status = GS_ACHIEVED;
			BWAPI::Broodwar->printf("Goal achieved");
		}
	}
}

GoalPurpose Goal::getPurpose() const
{
    return purpose;
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

GoalType Goal::getType() const{
	return type;
}