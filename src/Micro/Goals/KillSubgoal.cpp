#include "KillSubgoal.h"

KillSubgoal::KillSubgoal(SubgoalLogic l, Unit* u) : 
Subgoal(l),
target(u)
{

}
bool KillSubgoal::isRealized(){
	return this->check();

}

bool KillSubgoal::check(){
return false;
}

double KillSubgoal::distanceToRealize(UnitsGroup *ug){
return 0;

}

void KillSubgoal::tryToRealize(UnitsGroup * ug){


}