#include <PrecompiledHeader.h>
#include "Micro/Goals/GoalManager.h"

using namespace BWAPI;
using namespace std;

GoalManager::GoalManager()
: _firstPoke(false) // hack, remove TODO
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

	/// Do a defend goal to regroup units and defend the base if nothing else to do, very low priority (10)
	if (_goals.empty())
		_goal.push_back(pGoal(new DefendGoal(BWTA::getStartLocation(Broodwar->self()), 10)));

	if (!_firstPoke && ScoutController::Instance().enemyFound &&
		(Broodwar->enemy()->getRace() == Races::Zerg && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Zealot) >= 2)
		|| (Broodwar->enemy()->getRace() != Races::Zerg && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) >= 2)
    {
        ug->addGoal(pGoal(new AttackGoal(ug, ScoutController::Instance().enemyStartLocation)));
		_firstPoke = true;
	} 

	/// Update all goals
	for each (pGoal g in _goals)
		g->update();
	/// Removed finished goals
	for (list<pGoal>::const_iterator it = _goals.begin();
		it != _goals.end();)
	{
		if ((*it)->getStatus() == GS_ACHIEVED || (*it)->getStatus() == GS_CANCELED)
			_goals.erase(it++);
		else
			++it;
	}
}

void WarManager::sendGroupToAttack(UnitsGroup* ug)
{
	if (ScoutController::Instance().enemyFound)
    {
        ug->addGoal(pGoal(new AttackGoal(ug, ScoutController::Instance().enemyStartLocation)));
	} 
	else 
	{
		for (std::set<BWTA::BaseLocation*>::const_iterator it = BWTA::getStartLocations().begin();
			it != BWTA::getStartLocations().end(); ++it)
		{
			if (*it != BWTA::getStartLocation(Broodwar->self()))
				ug->addGoal(pGoal(new AttackGoal(ug, (*it)->getPosition())));
		}
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
