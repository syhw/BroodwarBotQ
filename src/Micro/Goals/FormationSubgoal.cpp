#include "FormationSubgoal.h"	
using namespace BWAPI;
using namespace std;

<<<<<<< HEAD
#define _OK_DISTANCE_ 16.0
=======
#define _OK_DISTANCE_ 4.0 // should perhaps be (WALTILE/2)^2 = 16.0
>>>>>>> essai

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
<<<<<<< HEAD
    Position p = Position((int)formation->center.x, (int)formation->center.y);
    return p.getDistance(unitsGroup->center);
=======
    Position p = Position((int)formation->mean.x, (int)formation->mean.y);
    return p.getDistance(unitsGroup->center);    
>>>>>>> essai
}

bool FormationSubgoal::check()
{
<<<<<<< HEAD
    if (distanceToRealize() <= unitsGroup->size()*_OK_DISTANCE_) 
        return true;
    //Broodwar->printf("dist: %f", distanceToRealize(unitsgroup));
=======
    if (distanceToRealize() <= sqrt(unitsGroup->size()*unitsGroup->size()*_OK_DISTANCE_)) 
        return true;
    //Broodwar->printf("dist: %f", distanceToRealize());
>>>>>>> essai
    return false;
}
