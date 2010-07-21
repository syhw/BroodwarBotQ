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
	if (regions->enemyFound()){
#ifdef __DEBUG_LOUIS__
		BWAPI::Broodwar->printf("findSubgoal done");
#endif
		return true;
	}
	return false;
//	return regions->enemyFound();
}

void FindSubgoal::tryToRealize(){
}

double FindSubgoal::distanceToRealize(){
	return -1;
}