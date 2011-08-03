#include <PrecompiledHeader.h>
#include "Micro/Goals/DropGoal.h"
#include "Subgoal.h"

using namespace BWAPI;
using namespace std;

/// TODO Refactor this: instead of not using the units group, do a
/// BasicUnitsGroup
/// DropUnitsGroup : BasicUnitsGroup
/// FightUnitsGroup : BasicUnitsGroup
/// and have the a BasicUnitsGroup* in Goal to runtime dispatch the good update()
/// instead of hacking in here (nonFlyers / switchMode(MODE_SCOUT)...)

DropGoal::DropGoal(Position p, const map<UnitType, int>& nU, int priority)
: Goal(nU, priority)
, _dropPos(p)
{
}

void DropGoal::achieve()
{

	_unitsGroup.nonFlyers = 0;
	_unitsGroup.switchMode(MODE_SCOUT);
}