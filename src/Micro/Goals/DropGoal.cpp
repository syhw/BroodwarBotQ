#include <PrecompiledHeader.h>
#include "Micro/Goals/DropGoal.h"
#include "Subgoal.h"

using namespace BWAPI;
using namespace std;

DropGoal::DropGoal(Position p, const map<UnitType, int>& nU, int priority)
: Goal(nU, priority)
, _dropPos(p)
{
}

void DropGoal::achieve()
{
}