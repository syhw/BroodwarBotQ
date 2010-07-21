#include "FormationSubgoal.h"	
using namespace BWAPI;
using namespace std;

#define _OK_DISTANCE_ 16.0

FormationSubgoal::FormationSubgoal(SubgoalLogic l, pFormation f): 
Subgoal(l),
formation(f)
{ 
}

bool FormationSubgoal::isRealized()
{
    return check();
}

void FormationSubgoal::tryToRealize()
{
    unitsGroup->formation(formation);
}

double FormationSubgoal::distanceToRealize()
{
    Position p = Position((int)formation->center.x, (int)formation->center.y);
    return p.getDistance(unitsGroup->center);
}

bool FormationSubgoal::check()
{
    if (distanceToRealize() <= unitsGroup->size()*_OK_DISTANCE_) 
        return true;
    //Broodwar->printf("dist: %f", distanceToRealize(unitsgroup));
    return false;
}
