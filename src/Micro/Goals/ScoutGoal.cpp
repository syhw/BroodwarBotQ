#include "ScoutGoal.h"


ScoutGoal::ScoutGoal():Goal(GT_SCOUT){
	regions = & Regions::Instance();
}


ScoutGoal::~ScoutGoal(){
}


void ScoutGoal::checkAchievement(UnitsGroup* ug){
	if(regions->EnemyFound())
		this->status=GS_ACHIEVED;
}