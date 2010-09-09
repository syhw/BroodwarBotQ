#pragma once
#include <boost/shared_ptr.hpp>
class Goal;
typedef boost::shared_ptr<Goal> pGoal;

#include "Subgoal.h"
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "Formations.h"
#include <list>

class UnitsGroup;
class Subgoal;

typedef enum
{
	GS_ND_STATUS           = 0,
	GS_ACHIEVED            = 0,
	GS_IN_PROGRESS         = 1, //When attributed
	GS_NOT_ATTRIBUTED      = 2,
	GS_NOT_ENOUGH_UNITS    = 3
} GoalStatus;

class Goal
{
protected:
	UnitsGroup * unitsGroup;        //Use setUnitsGroup
	std::list<pSubgoal> subgoals;   //The subgoals cannot be shared
	GoalStatus status;              /**< status of the goal */
    int firstFrame;

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
