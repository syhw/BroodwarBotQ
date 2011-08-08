#include <PrecompiledHeader.h>
#include "Micro/Goals/DefendGoal.h"
#include "Subgoal.h"
#include "FormationSubgoal.h"
#include "Micro/Formations.h"
#include "Macro/BWSAL.h"
#include "Macro/BorderManager.h"
#include "Micro/Goals/RegroupGoal.h"
#include "Micro/Micro.h"
#include "Macro/UnitGroupManager.h"

using namespace BWAPI;
using namespace std;

#define __BAIT_WORKERS_TILES_DISTANCE__ 8

//TODO Refactor

DefendGoal::DefendGoal(BWAPI::Position p, int priority, int firstFrame)
: Goal(priority,
	   firstFrame)
, _defPos(p)
, _choke(NULL)
, _base(BWTA::getNearestBaseLocation(_defPos))
, _eUnits(0) // 1?
, _nbWorkers(0)
{
	_status = GS_IN_PROGRESS;
}

DefendGoal::DefendGoal(BWTA::Chokepoint* c, int priority, int firstFrame)
: Goal(priority,
	   firstFrame)
, _defPos(c->getCenter())
, _choke(c)
, _base(BWTA::getNearestBaseLocation(_defPos))
, _eUnits(0) // 1?
, _nbWorkers(0)
{
	_status = GS_IN_PROGRESS;
}

DefendGoal::DefendGoal(BWTA::BaseLocation* b, int priority, int firstFrame)
: Goal(priority,
	   firstFrame)
, _defPos(b->getPosition())
, _choke(NULL)
, _base(b)
, _eUnits(0) // 1?
, _nbWorkers(0)
{
	_status = GS_IN_PROGRESS;
}

void DefendGoal::check()
{
	if (!Micro::Instance().needDefense.count(_base))
		_status = GS_ACHIEVED;
	else
	{
		bidDefUnits();

		_status = GS_IN_PROGRESS;

		set<Unit*> unitsAround = Broodwar->getUnitsInRadius(_defPos, __TILES_RADIUS_DEFEND_BASE__ * TILE_SIZE); // could be another radius 
#ifdef __DEBUG__
		Broodwar->drawCircleMap(_defPos.x(), _defPos.y(), __TILES_RADIUS_DEFEND_BASE__ * TILE_SIZE, Colors::Red); // def aggro zone
#endif
		// Perhaps use SelectAll()?
		_eUnits = 0;
		for each (Unit* u in unitsAround)
		{
			if (u->getPlayer() != Broodwar->enemy())
				continue;
			/// Count how many probes we will be ok to bid and how much force we need
			UnitType ut = u->getType();
			if (ut == UnitTypes::Terran_Dropship || ut == UnitTypes::Protoss_Shuttle || ut == UnitTypes::Zerg_Overlord
				|| ut == UnitTypes::Protoss_Dark_Templar || ut == UnitTypes::Zerg_Lurker)
				; // TODO
			//_eUnits += 16; // (supply is *2, 2 for a marine for instance)
			else if ((ut == UnitTypes::Protoss_Probe || ut == UnitTypes::Terran_SCV || ut == UnitTypes::Zerg_Drone)
				&& (u->isAttacking() || u->isConstructing())) //|| u->getTarget()->getPlayer() == Broodwar->self()))
				_eUnits += ut.supplyRequired(); // 2 probes on one attacking worker
			else if (ut == UnitTypes::Zerg_Zergling)
				_eUnits += 3*(ut.supplyRequired()); // 3 probes per zergling (zerglings are 1 supply each)
			else 
				_eUnits += ut.supplyRequired();
		}
		for each (pBayesianUnit bu in _unitsGroup.units)
		{
			if (bu->getType().isWorker() && !bu->isFighting())
			{
				TheArbitrator->removeBid(this, bu->unit);
				_biddedOn.erase(bu->unit);
			}
		}
		list<Unit*> toRem;
		/*while (!_workersOwned.empty()
			&& (_workersOwned.size() > _eUnits))
		{
			TheArbitrator->removeBid(this, _workersOwnded.front());
			_workersOwned.pop_front();
		}*/
	}
}

void DefendGoal::achieve()
{
	check(); // TODO mode in update();
	if (_unitsGroup.getDistance(_defPos) > 40*TILE_SIZE) // TODO 40 magic number
	{
		if (_unitsGroup.groupMode != MODE_MOVE)
			_unitsGroup.switchMode(MODE_MOVE);
	}
	else
	{
		if (_unitsGroup.groupMode != MODE_FIGHT_G)
			_unitsGroup.switchMode(MODE_FIGHT_G);
	}
	_unitsGroup.update();
}

std::string DefendGoal::getName() const
{
	return "DefendGoal";
}

std::string DefendGoal::getShortName() const
{
	return "DefG";
}

void DefendGoal::bidDefUnits()
{
	set<Unit*> interestingUnits = SelectAll().not(isBuilding);
	for each (Unit* u in interestingUnits)
	{
		if (!_biddedOn.count(u) && (!u->getType().isWorker() || (u->getDistance(_defPos) < __BAIT_WORKERS_TILES_DISTANCE__*TILE_SIZE && _nbWorkers < _eUnits)))
		{
			// units at 20 build tiles will be bidded on at 70, really important goals will have a priority > 90
			int prio = max(_priority, 90 - (int)(u->getDistance(_defPos)/TILE_SIZE)); 
			TheArbitrator->setBid(this, u, prio);
			_biddedOn.insert(u);
		}
	}
}

void DefendGoal::onOffer(set<Unit*> objects) // TODO refactor (only a 3 lines diff with Goal::onOffer)
{
	GoalManager* gm = & GoalManager::Instance();
	if (_status == GS_WAIT_PRECONDITION || _status == GS_IN_PROGRESS)
	{
        for each (Unit* u in objects)
		{
			if (!u->getType().isBuilding() && _nbWorkers < _eUnits)
			{
				if (u->getType().isWorker())
					++_nbWorkers;
				TheArbitrator->accept(this, u, _priority);
				if (gm->getCompletedUnits().find(u) != gm->getCompletedUnits().end())
				{
					if (_neededUnits.find(u->getType()) != _neededUnits.end())
						_neededUnits[u->getType()] -= 1;
					_unitsGroup.dispatchCompleteUnit(gm->getCompletedUnit(u));
				}
				else
					_incompleteUnits.push_back(u);
			}
		}
	}
	else
	{
		TheArbitrator->decline(this, objects, 0);
		TheArbitrator->removeBid(this, objects);
        for each (Unit* u in objects)
			_biddedOn.erase(u);
	}
}

void DefendGoal::onRevoke(Unit* u, double bid)
{
	onUnitDestroy(u);
}

void DefendGoal::onUnitDestroy(BWAPI::Unit* unit)
{
	if (unit->getPlayer() == Broodwar->self() && unit->getType().isWorker())
		--_nbWorkers;
	_unitsGroup.onUnitDestroy(unit);
	if (_status == GS_WAIT_PRECONDITION && _neededUnits.find(unit->getType()) != _neededUnits.end())
		_neededUnits[unit->getType()] += 1; // no effect on the goal if we are not in GS_WAIT_PRECONDITION
}

void DefendGoal::update()
{
	Goal::update();
}