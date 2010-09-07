#include "SeeSubgoal.h"
	
SeeSubgoal::SeeSubgoal(SubgoalLogic l, BWAPI::Position pos)
: Subgoal(l)
, achieved(false)
, pos(pos)
{
}

SeeSubgoal::SeeSubgoal(const SeeSubgoal &ssg)
: Subgoal(ssg.logic)
, achieved(ssg.achieved)
, pos(ssg.pos)
{
}

bool SeeSubgoal::isRealized()
{
	return check();
}

bool SeeSubgoal::check()
{
	return BWAPI::Broodwar->isVisible(pos.x()/32, pos.y()/32);
}

void SeeSubgoal::tryToRealize()
{
	unitsGroup->move(pos);
	unitsGroup->switchMode(MODE_SCOUT);
}

double SeeSubgoal::distanceToRealize()
{
	return unitsGroup->getCenter().getDistance(pos); 
}

double SeeSubgoal::distanceToRealize(BWAPI::Position p)
{
	return p.getDistance(pos);
}