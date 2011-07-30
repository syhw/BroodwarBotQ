#include <PrecompiledHeader.h>
#include "Micro/Goals/GoalManager.h"

using namespace BWAPI;
using namespace std;

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
		if ((*it)->isCompleted())
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
	for (list<pGoal>::const_iterator it = _goals.begin();
		it != _goals.end();)
	{
		if ((*it)->getStatus() == GS_ACHIEVED || (*it)->getStatus() == GS_CANCELED)
			_goals.erase(it++);
		else
			++it;
	}
}

void GoalManager::addGoal(pGoal g)
{
	_goals.push_back(g);
}

/// Add all units not building
void GoalManager::onUnitCreate(Unit* u)
{
	if (!u->getType().isBuilding()) // yup, even workers
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

const map<Unit*, pBayesianUnit>& GoalManager::getCompletedUnits() const
{
	return _completedUnits;
}

pBayesianUnit GoalManager::getCompletedUnit(Unit* u)
{
	return _completedUnits[u];
}
