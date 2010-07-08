#include "FindSubgoal.h"
	
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
		check();
		return achieved;
}

bool FindSubgoal::check(){
	return regions->EnemyFound();	
}

void FindSubgoal::tryToRealize(UnitsGroup * ug){
}

double FindSubgoal::distanceToRealize(UnitsGroup *ug){
	return -1;
}