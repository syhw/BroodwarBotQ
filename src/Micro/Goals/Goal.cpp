#include <PrecompiledHeader.h>
#include <algorithm>
#include "Micro/Goals/Goal.h"
#include "Macro/BWSAL.h"
#include "Macro/UnitGroupManager.h"
#include "Defines.h"

using namespace std;
using namespace BWAPI;

Goal::~Goal()
{
#ifdef __MICRO_DEBUG__
	clock_t start = clock();
#endif
	TheArbitrator->removeController(this);
#ifdef __MICRO_DEBUG__
	clock_t end = clock();
	double time = (double)(end - start)/CLOCKS_PER_SEC;
	if (time > 0.01)
		Broodwar->printf("\x07Goal destructor (removeController) took %f sec", time);
#endif
}

Goal::Goal(int priority, int firstFrame)
: _status(GS_WAIT_PRECONDITION)
, _priority(priority)
, _firstFrame(firstFrame)
, _neededUnits(std::map<UnitType, int>())
, _unitsGroup(UnitsGroup())
, _firstActive(0)
{
	if (!firstFrame)
		_firstFrame = Broodwar->getFrameCount();
	_unitsGroup.switchMode(MODE_MOVE);
}

Goal::Goal(pSubgoal s, int priority, int firstFrame)
: _status(GS_WAIT_PRECONDITION)
, _priority(priority)
, _firstFrame(firstFrame)
, _neededUnits(std::map<UnitType, int>())
, _unitsGroup(UnitsGroup())
, _firstActive(0)
{
	addSubgoal(s);
	if (!firstFrame)
		_firstFrame = Broodwar->getFrameCount();
	_unitsGroup.switchMode(MODE_MOVE);
}

Goal::Goal(const map<UnitType, int>& nU, pSubgoal s,
		   int priority, int firstFrame)
: _status(GS_WAIT_PRECONDITION)
, _priority(priority)
, _firstFrame(firstFrame)
, _neededUnits(std::map<UnitType, int>())
, _unitsGroup(UnitsGroup())
, _firstActive(0)
{
	_neededUnits = nU;
	addSubgoal(s);
	if (!firstFrame)
		_firstFrame = Broodwar->getFrameCount();
	_unitsGroup.switchMode(MODE_MOVE);
}

Goal::Goal(const std::map<BWAPI::UnitType, int>& nU,
	 int priority, int firstFrame)
: _status(GS_WAIT_PRECONDITION)
, _priority(priority)
, _firstFrame(firstFrame)
, _neededUnits(std::map<UnitType, int>())
, _unitsGroup(UnitsGroup())
, _firstActive(0)
{
	_neededUnits = nU;
	if (!firstFrame)
		_firstFrame = Broodwar->getFrameCount();
	_unitsGroup.switchMode(MODE_MOVE);
}

void Goal::bidOnUnitType(const UnitType& ut)
{
	set<Unit*> tmp = SelectAll(Broodwar->self(), ut);
	for each (Unit* u in tmp)
	{
		bidOnUnit(u);
	}
}

void Goal::bidOnMilitaryUnits()
{
	set<Unit*> tmp = SelectAll().not(isWorker).not(isBuilding);
	for each (Unit* u in tmp)
	{
		bidOnUnit(u);
	}
}

void Goal::bidOnUnit(Unit* u)
{
	if (!_biddedOn.count(u))
	{
		TheArbitrator->setBid(this, u, _priority);
		_biddedOn.insert(u);
	}
}

void Goal::onOffer(set<Unit*> objects)
{
	GoalManager* gm = & GoalManager::Instance();
	if (_status == GS_WAIT_PRECONDITION || _status == GS_IN_PROGRESS)
	{
        for each (Unit* u in objects)
		{
			if (!u->getType().isBuilding() && !u->getType().isWorker())
			{
				TheArbitrator->accept(this, u, _priority);
				pBayesianUnit tmp = gm->getCompletedUnit(u);
				if (tmp)
				{
					if (_neededUnits.find(u->getType()) != _neededUnits.end())
						_neededUnits[u->getType()] -= 1;
					_unitsGroup.dispatchCompleteUnit(gm->getCompletedUnit(u));
				}
				else
					_incompleteUnits.push_back(u);
			}
			else
			{
				TheArbitrator->decline(this, u, 0);
				TheArbitrator->removeBid(this, u);
				_biddedOn.erase(u);
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

void Goal::onRevoke(Unit* u, double bid)
{
	onUnitDestroy(u);
}

void Goal::onUnitDestroy(BWAPI::Unit* unit)
{
	_unitsGroup.onUnitDestroy(unit);
	if (_status == GS_WAIT_PRECONDITION && _neededUnits.find(unit->getType()) != _neededUnits.end())
		_neededUnits[unit->getType()] += 1; // no effect on the goal if we are not in GS_WAIT_PRECONDITION
}

string Goal::getName() const
{
	return "Goal";
}

string Goal::getShortName() const
{
	return "Goal";
}

/***
 * Dispatch bid units / achieve() / cancel() / removeAllBids()
 * w.r.t. the Goal _status
 */
void Goal::update()
{
#ifdef __ARBITRATOR_DEBUG__
	for each (pBayesianUnit bu in _unitsGroup.units)
	{
		Position displayp = bu->unit->getPosition();
		Broodwar->drawTextMap(max(0, displayp.x() - 16), min(Broodwar->mapHeight()*TILE_SIZE - 8, displayp.y() + 16), "\x07 %d", this);
	}
#endif

	/// Update incomplete units
	for (list<Unit*>::const_iterator it = _incompleteUnits.begin();
		it != _incompleteUnits.end(); )
	{
		if ((*it) == NULL || !(*it)->exists())
		{
			_incompleteUnits.erase(it++);
		}
		else
		{
			if ((*it)->isCompleted())
			{	
				if (_neededUnits.find((*it)->getType()) != _neededUnits.end())
					_neededUnits[(*it)->getType()] -= 1;
				GoalManager* gm = & GoalManager::Instance();
				pBayesianUnit tmp = gm->getCompletedUnit(*it);
				if (tmp)
					_unitsGroup.dispatchCompleteUnit(tmp);
				_incompleteUnits.erase(it++);
			}
			else
				++it;
		}
	}

	if (_status == GS_WAIT_PRECONDITION)
	{
		/// Wait for the first frame trigger (if existing)
		if (Broodwar->getFrameCount() < _firstFrame)
			return;
		/// Request needed units
		bool gotAllUnits = true;
		for (map<UnitType, int>::const_iterator it = _neededUnits.begin();
			it != _neededUnits.end(); ++it)
		{
			if (it->second > 0)
			{
				gotAllUnits = false;
				bidOnUnitType(it->first);
			}
		}
		if (_unitsGroup.size() && gotAllUnits)
			_status = GS_IN_PROGRESS;
	}
	else if (_status == GS_IN_PROGRESS)
		this->achieve();
	else if (_status == GS_IN_CANCEL)
		this->cancel();
	//else if (_status == GS_CANCELED || _status == GS_ACHIEVED)
	else
		TheArbitrator->removeAllBids(this);
}

/***
 * Do what it can to achieve the Goal:
 * first check() for achievement (this can be omitted in overloaded achieve())
 * [lazy and] takes the shortest distanceToRealize() Subgoal and tryToRealize()
 */
void Goal::achieve()
{
	if (_status != GS_IN_PROGRESS) // defensive
		return;
	/// Check for the goal achievement
	check(); // TODO move in update
	//////////////////////////////////
	if (_status == GS_ACHIEVED)
		return;
	/// Select the Subgoal which is faster to realize 
	pSubgoal selected;
	double min = DBL_MAX;			
	for (list<pSubgoal>::const_iterator it = _subgoals.begin();
		it != _subgoals.end(); ++it)
	{
		if (!(*it)->isRealized())
		{
			double test = (*it)->distanceToRealize();
			if (test >= 0 && test < min)
			{
				min = test;
				selected = (*it);
			}
		}
	}
	if (min > 0 && min < DBL_MAX)
	{
		selected->tryToRealize();
	} 
	else 
	{
		Broodwar->printf("no selected SubGoal but goal not achieved");
		// Bug if here
	}

	/// Default behavior of the goal is to update the _unitsGroup in achieve/cancel
	_unitsGroup.update();
}

/***
 * Check for achievement:
 * All the _subgoals are tested 
 * (there can be either one SL_OR true or all the SR_AND)
 */
void Goal::check()
{
	if (_firstActive && Broodwar->getFrameCount() - _firstActive > 4320) // 3 minutes
		_status = GS_CANCELED;
	if (_status == GS_ACHIEVED || _status == GS_CANCELED)
		return;
	if (!_firstActive && _status == GS_IN_PROGRESS)
		_firstActive = Broodwar->getFrameCount();
	bool and_goals = false;
	bool res_and = true;
	bool res_or = false;
	for (list<pSubgoal>::iterator p = _subgoals.begin(); p != _subgoals.end(); )
	{
		if ((*p)->getLogic() == SL_AND)
		{
			//AND case 
			and_goals = true; // there is as least one AND goal
			if (!(*p)->isRealized())
			{
				res_and = false;
				++p;
			} 
			else
				_subgoals.erase(p++);
		} else {
			//OR case
			if ((*p)->isRealized())
				res_or = true;
			++p;
		}
	}

	if (res_or || (and_goals && res_and))
	{
		_status= GS_ACHIEVED;
#ifdef __MICRO_DEBUG__
		BWAPI::Broodwar->printf("\x13 \x04 goal finished in ~ %d seconds", (BWAPI::Broodwar->getFrameCount() - _firstFrame)/24);
#endif
	}
}

void Goal::cancel()
{
	/// Does nothing, to be overwritten in cancelable goals
	/// Default behavior of the goal is to update the _unitsGroup in achieve/cancel
	_unitsGroup.update();
}

void Goal::addSubgoal(pSubgoal s)
{
	_subgoals.push_back(s);
	s->setUnitsGroup(&_unitsGroup);
}

void Goal::setStatus(GoalStatus s)
{
	_status = s;
}

GoalStatus Goal::getStatus() const
{
	return _status;
}
