#pragma once
#include <boost/shared_ptr.hpp>
class Goal;
typedef boost::shared_ptr<Goal> pGoal;

#include "Subgoal.h"
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "Micro/Formations.h"
#include <list>

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
protected:
	/// Units it can use
	UnitsGroup unitsGroup;
	/// Preconditions
	std::map<UnitType, int> neededUnits;
    int firstFrame;
	/// Subgoals
	std::list<pSubgoal> subgoals;   //The subgoals cannot be shared
    /// Status
	GoalStatus status;

public:
	void setUnitsGroup(UnitsGroup * ug);

	//Constructors
	Goal();//Don't forget to set the unitsGroup
	Goal(UnitsGroup * ug);
    Goal(UnitsGroup * ug, pSubgoal s);
	virtual ~Goal();
	
	virtual void achieve();//Start the goal

	virtual void checkAchievement();//Check if accomplished
	//(Check if all subgoals are accomplished)

	//Mutators
	void addSubgoal(pSubgoal s);
	void setStatus(GoalStatus s);
	virtual int estimateDistance(BWAPI::Position);
	//Accessors
	GoalStatus getStatus() const;
};
