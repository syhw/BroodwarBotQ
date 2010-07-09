#include "FindSubgoal.h"
#include "Defines.h"
FindSubgoal::FindSubgoal(SubgoalLogic l) :
Subgoal(l),
achieved(false)
{
	regions = & Regions::Instance();
}


bool FindSubgoal::isRealized(){
	if(achieved)
		return true;
	else
		achieved=check();
		return achieved;
}

bool FindSubgoal::check(){
	if (regions->EnemyFound()){
#ifdef __DEBUG__LOUIS
		BWAPI::Broodwar->printf("findSubgoal done");
#endif
		return true;
	}
	return false;
//	return regions->EnemyFound();
}

void FindSubgoal::tryToRealize(UnitsGroup * ug){
}

double FindSubgoal::distanceToRealize(UnitsGroup *ug){
	return -1;
}