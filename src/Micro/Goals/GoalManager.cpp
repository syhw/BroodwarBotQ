#include <PrecompiledHeader.h>
#include "Micro/Goals/GoalManager.h"

using namespace BWAPI;
GoalManager::GoalManager()
{
}

GoalManager::~GoalManager()
{
}

void GoalManager::update()
{
	/// Check is units in training are completed and create BayesianUnits if so
	for (list<Unit*>::const_iterator it = _inTrainingUnits.begin();
		it != _inTrainingUnits.end(); )
	{
		if (u->isCompleted())
		{
			pBayesianUnit tmp = BayesianUnit::newBayesianUnit(*it);
			if (tmp.get() != NULL)
				_completedUnits.insert(make_pair<Unit*, pBayesianUnit>(*it, tmp));
			_inTrainingUnits.erase(it++);
		}
		else
			++it;
	}
	/// Update all goals
	for each (pGoal g in _goals)
		g->update();
}

void GoalManager::addGoal(pGoal g)
{
	_goals.push_back(g);
}

/// Add all units not building
void GoalManager::onUnitCreate(Unit* u)
{
	if (!u->isBuilding()) // yup, even workers
		_inTrainingUnits.push_back(u);
}

/// Remove from _completedUnits or _inTrainingUnits
void GoalManager::onUnitDestroy(Unit* u)
{
	for (map<Unit*, pBayesianUnit>::const_iterator it = _completedUnits.begin();
		it != _completedUnits.end(); ++it)
	{
		if (it->first == u)
		{
			_completedUnits.erase(it);
			return;
		}
	}
	_inTrainingUnits.remove(u);
}