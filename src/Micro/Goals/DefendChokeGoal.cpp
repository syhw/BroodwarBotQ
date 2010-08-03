#include "DefendChokeGoal.h"
#include "SeeSubgoal.h"

DefendChokeGoal::DefendChokeGoal(UnitsGroup * ug,BWTA::Chokepoint * choke) : 
Goal(ug, pSubgoal(new SeeSubgoal(SL_AND,choke->getCenter())))
{


}



DefendChokeGoal::~DefendChokeGoal(){

}
	