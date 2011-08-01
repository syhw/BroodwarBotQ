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
    if (distanceToRealize() <= sqrt(_unitsGroup->size() * _unitsGroup->size() * _OK_DISTANCE_)) 
        return true;
    return false;
}

void FormationSubgoal::tryToRealize()
{
    _unitsGroup->formation(formation);
}

double FormationSubgoal::distanceToRealize()
{
    Position p = Position((int)formation->mean.x, (int)formation->mean.y);
    return p.getApproxDistance(_unitsGroup->center);
}