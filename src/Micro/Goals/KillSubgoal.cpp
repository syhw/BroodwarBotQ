#include <PrecompiledHeader.h>
#include "Micro/Goals/KillSubgoal.h"
#include "Macro/InformationManager.h"

KillSubgoal::KillSubgoal(SubgoalLogic l, UnitsGroup* ug, Unit* u) 
: Subgoal(l, ug)
, _target(u)
{
}

bool KillSubgoal::isRealized()
{
	return _target != NULL && TheInformationManager->exists(_target);
}

double KillSubgoal::distanceToRealize()
{
	return -1;
}

void KillSubgoal::tryToRealize()
{
	/// If we call tryToRealize on a "-1" distanceToRealize Subgoal
	/// it should be the last and only Subgoal of the Goal
	if (_target->isVisible())
	{
		_unitsGroup->move(_target->getPosition());
		_unitsGroup->defaultTargetEnemy = _target;
	}
	else
		_unitsGroup->move(TheInformationManager->getLastPosition(_target));
}