#include <algorithm>
#include "AttackGoal.h"
#include "UnitsGroup.h"

using namespace BWAPI;


AttackGoal::AttackGoal()
: Goal(GT_ATTACK)
{
}

AttackGoal::AttackGoal(const AttackGoal& g)
: Goal( g)
{
}


AttackGoal::~AttackGoal()
{
}

AttackGoal::AttackGoal(BWAPI::Position p):Goal(GT_ATTACK){
	this->addSubgoal(pSubgoal(new Subgoal(ST_ATTACK,SC_ONCE, p)));
}

void AttackGoal::checkAchievement(UnitsGroup* ug)
{
	if( Broodwar->isVisible( (int)formation->center.x/32, (int)formation->center.y/32) &&
		!unit->exists())
	{
		status = GS_ACHIEVED;
	}
}

