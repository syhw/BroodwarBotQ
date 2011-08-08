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
	_status = GS_IN_PROGRESS;
	bidOnMilitaryUnits();
	GoalManager::Instance().attackGoals += 1;
	//if (_unitsGroup.groupMode != MODE_SCOUT)
	//	_unitsGroup.switchMode(MODE_SCOUT);
}

RegroupGoal::~RegroupGoal()
{
	GoalManager::Instance().attackGoals -= 1;
}

void RegroupGoal::achieve()
{
	//if (_unitsGroup.groupMode != MODE_SCOUT)
	//		_unitsGroup.switchMode(MODE_SCOUT);
	bidOnMilitaryUnits();
	//if (_unitsGroup.groupMode != MODE_MANAGED)
	//	_unitsGroup.switchMode(MODE_MANAGED);
	for each (pSubgoal s in _subgoals)
	{
		if (s->isRealized())
		{
			_status = GS_ACHIEVED;
			//attackGoalHere();
			return;
		}
		else
			s->tryToRealize();
	}
	_unitsGroup.update();
	/*if (!(Broodwar->getFrameCount() % 25))
	{
		for each (pBayesianUnit bu in _unitsGroup.units)
		{
			bu->unit->move(bu->target);
		}
	}*/
}

/*void RegroupGoal::attackGoalHere()
{
	if (Broodwar->isWalkable(_unitsGroup.center.x() / 8, _unitsGroup.center.y() / 8))
		GoalManager::Instance().addGoal(pGoal(new AttackGoal(_unitsGroup.center)));
}*/