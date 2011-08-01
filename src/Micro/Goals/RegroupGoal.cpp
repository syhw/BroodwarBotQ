#include <PrecompiledHeader.h>
#include "Micro/Goals/RegroupGoal.h"
#include "Subgoal.h"
#include "FormationSubgoal.h"
#include "Micro/Formations.h"

using namespace BWAPI;
using namespace std;

RegroupGoal::RegroupGoal(Position p, int priority, int firstFrame)
: Goal(priority, firstFrame)
{
	addSubgoal(pSubgoal(new FormationSubgoal(SL_OR, &_unitsGroup, 
		pFormation(new SquareFormation(p)))));
    // perhaps TODO bid on military units here
}

void RegroupGoal::canBidOn(Unit* u)
{
	bidOnUnit(u);
}