#include <PrecompiledHeader.h>
#include <algorithm>
#include "Micro/Goals/Goal.h"
#include "Micro/UnitsGroup.h"
#include "Defines.h"

using namespace std;

Goal::~Goal()
{
#ifdef __DEBUG__
	log("deleted a goal\n");
#endif
	TheArbitrator->removeAllBids(this);
}

Goal::Goal()
: _status(GS_WAIT_PRECONDITION)
, _firstFrame(BWAPI::Broodwar->getFrameCount())
{
#ifdef __DEBUG__
	log("created a goal\n");
#endif
}

Goal::Goal(pSubgoal s)
: _status(GS_WAIT_PRECONDITION)
, _firstFrame(BWAPI::Broodwar->getFrameCount())
{
#ifdef __DEBUG__
	log("created a goal\n");
#endif
	addSubgoal(s);
}

Goal::Goal(const std::<UnitType, int>& nU, pSubGoal s);
: _status(GS_WAIT_PRECONDITION)
, _firstFrame(BWAPI::Broodwar->getFrameCount())
{
#ifdef __DEBUG__
	log("created a goal\n");
#endif
	_neededUnits = nU;
	addSubgoal(s);
}

void Goal::update()
{
	if (_status == GS_WAIT_PRECONDITION)
	{
	}
	else if (_status == GS_IN_PROGRESS)
		this->achieve();
	else if (_status == GS_IN_CANCEL)
		this->cancel();
	else if (_status == GS_CANCELED)
	{
		TheArbitrator->removeAllBids();

	}
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
