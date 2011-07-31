#include <PrecompiledHeader.h>
#include "SeeSubgoal.h"
	
SeeSubgoal::SeeSubgoal(SubgoalLogic l, BWAPI::Position pos)
: Subgoal(l)
, _achieved(false)
, _pos(pos)
{
}

SeeSubgoal::SeeSubgoal(const SeeSubgoal &ssg)
: Subgoal(ssg._logic)
, _achieved(ssg._achieved)
, _pos(ssg._pos)
{
}

bool SeeSubgoal::isRealized()
{
    _achieved = _achieved || BWAPI::Broodwar->isVisible(_pos.x()/32, _pos.y()/32);
	return _achieved;
}

void SeeSubgoal::tryToRealize()
{
	_unitsGroup->move(_pos);
}

double SeeSubgoal::distanceToRealize()
{
	/// Does not use pathfinding and so is not very precise
	return _unitsGroup->getDistance(_pos); 
}

double SeeSubgoal::distanceToRealize(BWAPI::Position p)
{
	/// Does not use pathfinding and so is not very precise
	return p.getApproxDistance(_pos);
}