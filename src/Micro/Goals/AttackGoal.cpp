#include "AttackGoal.h"
#include "Subgoal.h"
#include "FormationSubgoal.h"
#include "Formations.h"

AttackGoal::AttackGoal(UnitsGroup* ug, BWAPI::Position p)
: Goal(ug, pSubgoal(new FormationSubgoal(SL_AND, pFormation(new SquareFormation(p)))))
{
}

void AttackGoal::achieve()
{
	//See if we need an intermediate subgoal : 
	double min = 99999999.9;
	for(std::list<pSubgoal>::iterator it = subgoals.begin(); it != subgoals.end(); ++it){
		if ((*it)->distanceToRealize() > 0 && (*it)->distanceToRealize() < min)
        {
			min = (*it)->distanceToRealize();
		}
	}
	if (min > 1337)
    {
		this->createMidSubgoal();
	}
	Goal::achieve();
}

void AttackGoal::createMidSubgoal()
{
    if (unitsGroup->ppath.empty())
        return;
	//Create an intermediate subgoal at half the way of the path of the unitsgroup
	this->addSubgoal(pSubgoal(new FormationSubgoal(SL_AND,pFormation(new SquareFormation(unitsGroup->ppath[unitsGroup->ppath.size() / 2])))));
}