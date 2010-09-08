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
	int	min = 99999999;
	for(std::list<pSubgoal>::iterator it = subgoals.begin(); it != subgoals.end(); ++it){
		if ((*it)->distanceToRealize() > 0 && (*it)->distanceToRealize() < min)
        {
			min = (*it)->distanceToRealize();
		}
	}
	if (min > 1250)
    {
		this->createMidSubgoal();
	}
	Goal::achieve();
}

void AttackGoal::createMidSubgoal()
{
    if (!this->unitsGroup->ppath.size())
        return;
	int cpt = 0;
	int stop = this->unitsGroup->ppath.size() / 2;
	BWAPI::Position pos;
	for(std::vector<BWAPI::Position>::const_iterator it = this->unitsGroup->leadingUnit->getPPath().begin(); it != this->unitsGroup->leadingUnit->getPPath().end(); ++it)
    {
		cpt++;
		if (cpt == stop)
        {
			pos = (*it);
		}
	}
	//Create an intermediate subgoal at half the way of the path of the unitsgroup
	this->addSubgoal(pSubgoal(new FormationSubgoal(SL_AND,pFormation(new SquareFormation(pos,Vec(1,0))))));
}