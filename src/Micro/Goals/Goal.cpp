#include <PrecompiledHeader.h>
#include <algorithm>
#include "Micro/Goals/Goal.h"

using namespace std;

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

Goal::Goal(const std::<UnitType, int>& nU, pSubgoal s,
		   int priority, int firstFrame);
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

virtual void Goal::onOffer(set<Unit*> objects)
{
	if (_status == GS_WAIT_PRECONDITION || _status == GS_IN_PROGRESS)
	{
        for each (Unit* u in objects)
		{
		if (GoalManager::Instance()._
		TheArbitrator->accept(this, objects, 95);
		}
	}
	else
	{
		TheArbitrator->decline(this, objects, 0);
		TheArbitrator->removeBid(this, object);
        for each (Unit* u in objects)
			_biddedOn.erase(u);
	}
}

virtual void Goal::onRevoke(Unit* u, double bid);
{
}

virtual string Goal::getName() const
{
	return "Goal";
}

void Goal::update()
{
	if (_status == GS_WAIT_PRECONDITION)
	{
		/// Wait for the first frame trigger (if existing)
		if (Broodwar->getFrameCount() < firstFrame)
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
		TheArbitrator->removeAllBids();
}

void Goal::achieve()
{
	if (_status == GS_WAIT_PRECONDITION)
		return;
	check();
	if (_status == GS_ACHIEVED)
		return;
	double test;
	pSubgoal selected;
	double min = DBL_MAX;			
	for (list<pSubgoal>::const_iterator it = subgoals.begin();
		it != subgoals.end(); ++it)
	{
		if (!(*it)->isRealized())
		{
			test = (*it)->distanceToRealize();
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
	} else {
		Broodwar->printf("no selected SubGoal but goal not achieved");
		//TODO
	}
}

void Goal::check()
{
	if (this->status == GS_ACHIEVED)
		return;
	//All the subgoals are tested because the check function might validate
	//some subgoals
	bool res_and=true;
	bool res_or=false;

	for (std::list<pSubgoal>::iterator p = subgoals.begin(); p != subgoals.end(); )
	{
		if ((*p)->getLogic() == SL_AND)
		{
			//AND case 
			if (!(*p)->isRealized())
			{
				res_and=false;
				++p;
			} else
				subgoals.erase(p++);
		} else {
			//OR case
			if ((*p)->isRealized())
				res_or = true;
			++p;
		}
	}

	if(res_and || res_or)
	{
		_status= GS_ACHIEVED;
#ifdef __DEBUG__
		BWAPI::Broodwar->printf("\x13 \x04 goal finished in ~ %d seconds", (BWAPI::Broodwar->getFrameCount() - firstFrame)/24);
#endif
	}
}

void Goal::addSubgoal(pSubgoal s)
{
	if (this->status != GS_NOT_ATTRIBUTED)
	{
		s->setUnitsGroup(unitsGroup);
	}
	this->subgoals.push_back(s);
}

GoalStatus Goal::getStatus() const
{
	return status;
}

void Goal::setStatus(GoalStatus s)
{
	status = s;
}



void Goal::setUnitsGroup(UnitsGroup * ug)
{
	this->status = GS_IN_PROGRESS;
	for (std::list<pSubgoal>::iterator it = this->subgoals.begin(); it != subgoals.end(); ++it)
	{
		(*it)->setUnitsGroup(ug);
	}
	this->unitsGroup = ug;
}


int Goal::estimateDistance(BWAPI::Position p)
{

	for (std::list<pSubgoal>::iterator it = this->subgoals.begin(); it != this->subgoals.end(); ++it)
	{
		if ((*it)->distanceToRealize(p) > 0)
		{
			return (int)(*it)->distanceToRealize(p);
		}
	}
	return -1;
}
