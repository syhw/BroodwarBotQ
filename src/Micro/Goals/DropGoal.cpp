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
DropGoal::DropGoal(BWTA::BaseLocation* b, const std::map<BWAPI::UnitType, int>& nU, int priority)
: Goal(nU, priority)
, _base(b)
{
	_neededUnits.insert(make_pair<UnitType, int>(UnitTypes::Protoss_Shuttle, 1));
	_dropPos = b->getPosition();
}

DropGoal::DropGoal(Position p, const map<UnitType, int>& nU, int priority)
: Goal(nU, priority)
, _dropPos(p)
{
	_neededUnits.insert(make_pair<UnitType, int>(UnitTypes::Protoss_Shuttle, 1));
	_base = BWTA::getNearestBaseLocation(_dropPos);
}

void DropGoal::achieve()
{
	_unitsGroup.nonFlyers = 0;
	_unitsGroup.switchMode(MODE_SCOUT);
}