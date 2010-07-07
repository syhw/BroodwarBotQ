#include "ScoutGoal.h"


ScoutGoal::ScoutGoal(ScoutPurpose sp):Goal(GT_SCOUT),purpose(sp){
	regions = & Regions::Instance();
}


ScoutGoal::~ScoutGoal(){
}


void ScoutGoal::checkAchievement(UnitsGroup* ug){
	if(regions->EnemyFound())
		this->status=GS_ACHIEVED;
}