#include <PrecompiledHeader.h>
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
}

double SeeSubgoal::distanceToRealize()
{
	/// Does not use pathfinding and so is not very precise
	return unitsGroup->getCenter().getApproxDistance(pos); 
}

double SeeSubgoal::distanceToRealize(BWAPI::Position p)
{
	/// Does not use pathfinding and so is not very precise
	return p.getDistance(pos);
}