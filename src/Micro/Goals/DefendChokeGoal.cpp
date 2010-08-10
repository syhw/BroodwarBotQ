#include "DefendChokeGoal.h"
#include "SeeSubgoal.h"

DefendChokeGoal::DefendChokeGoal(UnitsGroup * ug,BWTA::Chokepoint * choke) : 
Goal(ug)
{

}



DefendChokeGoal::~DefendChokeGoal(){

}

void DefendChokeGoal::achieve(){
	this->checkAchievement();
}

void DefendChokeGoal::checkAchievement(){
	//Check if we have the right amount of units. If not, set GS_NOT_ENOUGH_UNITS and return
	if(this->unitsGroup == NULL){
		//Strange...But must be avoided
	}else{
		if(this->unitsGroup->getNbUnits() <= 5){
			this->status = GS_NOT_ENOUGH_UNITS;
		}else{
			this->status = GS_ACHIEVED;
		}
	}

	//Create appropriate goal of formation in the chokepoint and accomplish it



}
	
bool DefendChokeGoal::enoughUnits(){


}