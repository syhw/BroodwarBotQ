#include <algorithm>
#include "DefendGoal.h"
#include "UnitsGroup.h"

using namespace BWAPI;

/*
DefendGoal::DefendGoal()
: Goal()
{
 type = GT_DEFEND_BASE;
 status = GS_ACHIEVED;
 purpose = "Defend";
}

DefendGoal::DefendGoal(const DefendGoal& g)
: Goal( g) 
{
 type = GT_DEFEND_BASE;
 status = GS_ACHIEVED;
 purpose = "Defend";
}
*/
DefendGoal::DefendGoal(const Position& position)
: Goal("Defend", pFormation(new SquareFormation( position)))
{
 type = GT_DEFEND_BASE;
 status = GS_ACHIEVED;
}

DefendGoal::~DefendGoal()
{
}

void DefendGoal::achieve(UnitsGroup* ug)
{
	ug->formation( this->formation);
}

void DefendGoal::checkAchievement(UnitsGroup* ug)
{
}
