#include <PrecompiledHeader.h>
#include "FormationSubgoal.h"	

using namespace BWAPI;
using namespace std;

#define _OK_DISTANCE_ 2.0 // should perhaps be (WALKTILE/2)^2 = 16.0

FormationSubgoal::FormationSubgoal(SubgoalLogic l, UnitsGroup* ug, pFormation f)
: Subgoal(l, ug)
, formation(f)
{ 
}

FormationSubgoal::FormationSubgoal(const FormationSubgoal& fsg)
: Subgoal(fsg)
, formation(fsg.formation)
{
}

bool FormationSubgoal::isRealized()
{
    if (!_unitsGroup->isFighting
		&& distanceToRealize() <= sqrt(_unitsGroup->size() * _unitsGroup->size() * _OK_DISTANCE_))
        return true;
    return false;
}

void FormationSubgoal::tryToRealize()
{
    _unitsGroup->formation(formation);
}

double FormationSubgoal::distanceToRealize()
{
	if (!_unitsGroup)
		return -1.0;
	if (_unitsGroup->distToTarget > 0.0) // _unitsGroup not in the same region as its target and distToTarget has been initialized
		return _unitsGroup->distToTarget;
	// else
    Position p = Position((int)formation->mean.x, (int)formation->mean.y);
	return _unitsGroup->getDistance(p);
}