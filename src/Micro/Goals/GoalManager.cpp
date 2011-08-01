#include <PrecompiledHeader.h>
#include "Micro/Goals/GoalManager.h"
#include "Micro/Goals/RegroupGoal.h"

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
	/// Check if units in training are completed and create BayesianUnits if so
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

	/// Do a regroup goal garbage collect units if nothing else to do, lowest bid
	/*if (_goals.empty() || _assignedUnits.size() < _completedUnits.size())
	{
        garbageCollector = pGoal(new RegroupGoal(BWTA::getStartLocation(Broodwar->self())->getPosition(), 1));
		_goal.push_back(garbageCollector);
	}*/

	/// Do a first poke with at least 2 zealots against Z and at least 2 goons against others
	/// Hackety hack! TODO remove
	/*if (!_firstPoke && ScoutController::Instance().enemyFound &&
		(Broodwar->enemy()->getRace() == Races::Zerg && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Zealot) >= 2)
		|| (Broodwar->enemy()->getRace() != Races::Zerg && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) >= 2)
    {
        ug->addGoal(pGoal(new AttackGoal(ug, ScoutController::Instance().enemyStartLocation)));
		_firstPoke = true;
	}*/ 

	/// Update all goals
	for each (pGoal g in _goals)
	{
		if (!unassignedUnits.empty())
			g->canBidOn(unassignedUnits);
		g->update();
	}
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

/*void WarManager::sendGroupToAttack(UnitsGroup* ug)
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
}*/

/***
 * Add the goal is not already present in _goals (O(#goals)), defensive thing
 */
void GoalManager::addGoal(pGoal g)
{
	for each (pGoal pg in _goals)
	{
		if (pg == g)
			return;
	}
	_goals.push_back(g);
}

void GoalManager::addGoals(const list<pGoal>& l)
{
	for each (pGoal g in l)
		addGoal(g);
}

/// Add all units not building
void GoalManager::onUnitCreate(Unit* u)
{
	if (u->getPlayer() != Broodwar->self())
		return;
	if (!u->getType().isBuilding()) // yup, even workers, we want to create BayesianUnits in case they have to fight
	{
		_inTrainingUnits.push_back(u);
		if (!u->getType().isWorker()) // military units only, because we won't propose workers, only specials goals will bid on them
			unassignedUnits.insert(u); // we are sure that is it unassigned to a goal
	}
}

/// Remove from _completedUnits and unassignedUnits or from _inTrainingUnits and unassignedUnits
void GoalManager::onUnitDestroy(Unit* u)
{
	if (u->getPlayer() == Broodwar->self())
	{
		for (map<Unit*, pBayesianUnit>::const_iterator it = _completedUnits.begin();
			it != _completedUnits.end(); ++it)
		{
			if (it->first == u)
			{
				_completedUnits.erase(it);
				unassignedUnits.erase(u);
				break;
			}
		}
		_inTrainingUnits.remove(u);
		unassignedUnits.erase(u);
	}
	for each (pGoal g in _goals)
		g->onUnitDestroy(u); // will delete the pBayesianUnit OR will remove enemy units from the bimap
}

const map<Unit*, pBayesianUnit>& GoalManager::getCompletedUnits() const
{
	return _completedUnits;
}

pBayesianUnit GoalManager::getCompletedUnit(Unit* u)
{
	return _completedUnits[u];
}
