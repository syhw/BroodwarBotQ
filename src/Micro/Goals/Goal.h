#pragma once
#include <boost/shared_ptr.hpp>

#ifndef GOAL_SMART_POINTER
#define GOAL_SMART_POINTER
class Goal;
typedef boost::shared_ptr<Goal> pGoal;
#endif

#include "Macro/Arbitrator.h"
#include "Micro/UnitsGroup.h"
#include "Subgoal.h"
#include <BWAPI.h>
#include <BWTA.h>
#include <list>
#include <map>

class BasicUnitsGroup;
class Subgoal;

typedef enum
{
	GS_WAIT_PRECONDITION   = 0,
	GS_IN_PROGRESS         = 1,
	GS_IN_CANCEL           = 2,
	GS_CANCELED            = 3,
	GS_ACHIEVED            = 4
} GoalStatus;

class Goal : Arbitrator::Controller<BWAPI::Unit*, double>
{
	friend class UnitsGroup;
protected:
	/// Units it can use
	UnitsGroup _unitsGroup;
	/// Preconditions
	std::map<UnitType, int> _neededUnits;
    int _firstFrame;
	/// Subgoals
	std::list<pSubgoal> _subgoals;   //The subgoals cannot be shared
    /// Status
	GoalStatus _status;

public:
	Goal();
	Goal(int firstFrame = 0);
	Goal(pSubgoal s);
	Goal(pSubgoal s, int firstFrame = 0);
	Goal(const std::map<BWAPI::UnitType, int>& nU, pSubGoal s);
	Goal(const std::map<BWAPI::UnitType, int>& nU, pSubGoal s, int firstFrame = 0);
	virtual ~Goal();
	
	void update();
	virtual void achieve();
	virtual void achieve();
	virtual void check();

	void addSubgoal(pSubgoal s);
	void addNeededUnits(const std::map<BWAPI::UnitType, int>& neededUnits);
	void setFirstFrame(int firstFrame);
	
	void setStatus(GoalStatus s);
	GoalStatus getStatus() const;
	virtual int estimateDistance(BWAPI::Position);

};
