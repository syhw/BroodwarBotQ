#include <PrecompiledHeader.h>
#include "Micro/Goals/GoalManager.h"
#include "Micro/Goals/RegroupGoal.h"
#include "Macro/BWSAL.h"
#include "Defines.h"

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
#ifdef __MICRO_TESTS_ONLY__
	for each (Unit* u in Broodwar->self()->getUnits())
	{
		if (_completedUnits.find(u) == _completedUnits.end())
		{
			pBayesianUnit tmp = BayesianUnit::newBayesianUnit(u);
			_completedUnits.insert(make_pair<Unit*, pBayesianUnit>(u, tmp));
		}
	}
#endif

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

#ifdef __DEBUG__
	int y = 3;
#endif
	/// Update all goals
	for each (pGoal g in _goals)
	{
		if (!unassignedUnits.empty())
			g->canBidOn(unassignedUnits);
		g->update();
#ifdef __DEBUG__
		if (g->getStatus() == GS_WAIT_PRECONDITION)
			Broodwar->drawTextScreen(5, y, "\x17%s", g->getName().c_str());
		else if (g->getStatus() == GS_IN_PROGRESS)
			Broodwar->drawTextScreen(5, y, "\x07%s", g->getName().c_str());
		else
			Broodwar->drawTextScreen(5, y, "%s", g->getName().c_str());
		y += 8;
#endif
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
		if (u->getType() == UnitTypes::Protoss_Scarab || u->getType() == UnitTypes::Protoss_Interceptor)
			return;
		_inTrainingUnits.push_back(u);
		if (!u->getType().isWorker()) // military units only, because we won't propose workers, only specials goals will bid on them
			TheArbitrator->setBid(this, u, 1);
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

/// Controller methods
void GoalManager::onOffer(std::set<BWAPI::Unit*> objects)
{
	for each (Unit* u in objects)
		unassignedUnits.insert(u);
}

void GoalManager::onRevoke(BWAPI::Unit* u, double bid)
{
	unassignedUnits.erase(u);
}

std::string GoalManager::getName() const
{
	return string("GoalManager");
}

std::string GoalManager::getShortName() const
{
	return string("GM");
}