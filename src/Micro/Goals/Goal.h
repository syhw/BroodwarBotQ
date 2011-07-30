#pragma once
#include <boost/shared_ptr.hpp>
#ifndef GOAL_SMART_POINTER
#define GOAL_SMART_POINTER
class Goal;
typedef boost::shared_ptr<Goal> pGoal;
#endif
#include "Subgoal.h"
#ifndef SUBGOAL_SMART_POINTER
#define SUBGOAL_SMART_POINTER
class Subgoal;
typedef boost::shared_ptr<Subgoal> pSubgoal;
#endif
#include "Macro/Arbitrator.h"
#include "Micro/UnitsGroup.h"
#include "Defines.h"
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
	std::map<BWAPI::UnitType, int> _neededUnits;
	std::set<BWAPI::Unit*> _biddedOn;
    int _priority;
    int _firstFrame;
	/// Subgoals
	std::list<pSubgoal> _subgoals;   //The subgoals cannot be shared
    /// Status
	GoalStatus _status;
	void bidOnUnit(const BWAPI::UnitType& ut);
public:
	Goal(int priority = 50);
	Goal(pSubgoal s, int priority = 50, int firstFrame = 0);
	Goal(const std::map<BWAPI::UnitType, int>& nU, pSubgoal s,
		int priority = 50, int firstFrame = 0);
	virtual ~Goal();
	
	virtual void onOffer(std::set<BWAPI::Unit*> objects);
	virtual void onRevoke(BWAPI::Unit* u, double bid);
    virtual std::string getName() const;
	virtual void update();
	virtual void achieve();
	virtual void check();

	void addSubgoal(pSubgoal s);
	void addNeededUnits(const std::map<BWAPI::UnitType, int>& neededUnits);
	void setFirstFrame(int firstFrame);
	void setPriority(int priority);
	
	void setStatus(GoalStatus s);
	GoalStatus getStatus() const;
	virtual int estimateDistance(BWAPI::Position);

};
