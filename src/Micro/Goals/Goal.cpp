#include <PrecompiledHeader.h>
#include <algorithm>
#include "Micro/Goals/Goal.h"
#include "Macro/BWSAL.h"
#include "Macro/UnitGroupManager.h"

using namespace std;
using namespace BWAPI;

Goal::~Goal()
{
#ifdef __DEBUG__
	log("deleted a goal\n");
#endif
	TheArbitrator->removeAllBids(this);
}

Goal::Goal(int priority)
: _status(GS_WAIT_PRECONDITION)
, _priority(priority)
, _firstFrame(BWAPI::Broodwar->getFrameCount())
{
#ifdef __DEBUG__
	log("created a goal\n");
#endif
}

Goal::Goal(pSubgoal s, int priority, int firstFrame)
: _status(GS_WAIT_PRECONDITION)
, _priority(priority)
, _firstFrame(firstFrame)
{
#ifdef __DEBUG__
	log("created a goal\n");
#endif
	addSubgoal(s);
	if (!firstFrame)
		_firstFrame = Broodwar->getFrameCount();
}

Goal::Goal(const map<UnitType, int>& nU, pSubgoal s,
		   int priority, int firstFrame)
: _status(GS_WAIT_PRECONDITION)
, _priority(priority)
, _firstFrame(firstFrame)
{
#ifdef __DEBUG__
	log("created a goal\n");
#endif
	_neededUnits = nU;
	addSubgoal(s);
	if (!firstFrame)
		_firstFrame = Broodwar->getFrameCount();
}

void Goal::bidOnUnitType(const UnitType& ut)
{
	set<Unit*> tmp = SelectAll(Broodwar->self(), ut);
	for each (Unit* u in tmp)
	{
		if (!_biddedOn.count(u))
		{
			TheArbitrator->setBid(this, u, _priority);
			_biddedOn.insert(u);
		}
	}
}

void Goal::onOffer(set<Unit*> objects)
{
	GoalManager* gm = & GoalManager::Instance();
	if (_status == GS_WAIT_PRECONDITION || _status == GS_IN_PROGRESS)
	{
        for each (Unit* u in objects)
		{
			if (gm->getCompletedUnits().find(u) != gm->getCompletedUnits().end())
			{
				TheArbitrator->accept(this, objects, _priority);
				if (_neededUnits.find(u->getType()) != _neededUnits.end())
					_neededUnits[u->getType()] -= 1;
				_unitsGroup.dispatchCompleteUnit(gm->getCompletedUnit(u));
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
	_unitsGroup.giveUpControl(u);
	if (_neededUnits.find(u->getType()) != _neededUnits.end())
		_neededUnits[u->getType()] += 1;
	///// TODO
}

string Goal::getName() const
{
	return "Goal";
}

void Goal::update()
{
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
		if (gotAllUnits)
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

void Goal::achieve()
{
	if (_status != GS_IN_PROGRESS) // defensive
		return;
	check();
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
}

/***
 * Check for achievement:
 * All the _subgoals are tested (there can be a SL_OR true)
 */
void Goal::check()
{
	if (_status == GS_ACHIEVED || _status == GS_CANCELED)
		return;
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

	if(res_or || (and_goals && res_and))
	{
		_status= GS_ACHIEVED;
#ifdef __DEBUG__
		BWAPI::Broodwar->printf("\x13 \x04 goal finished in ~ %d seconds", (BWAPI::Broodwar->getFrameCount() - _firstFrame)/24);
#endif
	}
}

void Goal::cancel()
{
}

void Goal::addSubgoal(pSubgoal s)
{
	_subgoals.push_back(s);
}

void Goal::setStatus(GoalStatus s)
{
	_status = s;
}

GoalStatus Goal::getStatus() const
{
	return _status;
}
