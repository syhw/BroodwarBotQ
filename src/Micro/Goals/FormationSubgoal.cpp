#include "FormationSubgoal.h"	
using namespace BWAPI;
using namespace std;

#define _OK_DISTANCE_ 16.0

FormationSubgoal::FormationSubgoal(SubgoalLogic l, pFormation f, UnitsGroup* ug): 
Subgoal(l),
formation(f),
unitsgroup(ug)
{ 
}

bool FormationSubgoal::isRealized()
{
    return check();
}

void FormationSubgoal::tryToRealize(UnitsGroup* ug)
{
    ug->formation(formation);
}

double FormationSubgoal::distanceToRealize(UnitsGroup* ug)
{
    Position p = Position((int)formation->center.x, (int)formation->center.y);
    return p.getDistance(ug->center);
}

bool FormationSubgoal::check()
{
    if (distanceToRealize(unitsgroup) <= unitsgroup->size()*_OK_DISTANCE_) 
        return true;
    //Broodwar->printf("dist: %f", distanceToRealize(unitsgroup));
    return false;
}
