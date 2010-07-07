#ifndef GOAL__H
#define GOAL__H
#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "Formations.h"
#include "Subgoal.h"
#include <boost/shared_ptr.hpp>

class Goal;
typedef boost::shared_ptr<Goal> pGoal;


class UnitsGroup;
typedef enum
{
	GT_UNDEFINED           = 0,
	GT_SCOUT               = 1,
	GT_ATTACK              = 2,
	GT_DEFEND              = 3

} GoalType;

typedef enum
{
	GS_ND_STATUS           = 0,
	GS_ACHIEVED            = 0,
	GS_IN_PROGRESS         = 1,
	GS_MOVE_TO             = 2,
	GS_FLEE                = 3,
	GS_NOT_STARTED         = 4
} GoalStatus;

class Goal
{
	//A goal contain a list of subgoals
	//Some subgoals must be validated only once (SC_ACTIVE),
	//other must be validated all along the accomplishment (SC_ONCE).
protected:
	std::list<pSubgoal> subgoals; //The subgoals cannot be shared
	GoalStatus status;      /**< status of the goal */
	GoalType type;
public:

	//Constructors
	Goal(); //Don't forgot to set a formation and movement if wanted
	//Else avoid enemy and void formation is used
	Goal(GoalType t);
	virtual ~Goal();
	
	virtual void achieve(UnitsGroup* ug);//Start the goal

	virtual void checkAchievement(UnitsGroup* ug);//Check if accomplished
	//(Check if all subgoals are accomplished)

	//Mutators
	void addSubgoal(pSubgoal s);

	//Accessors
	GoalStatus getStatus() const;
	void setStatus(GoalStatus s);
	GoalType getType() const;

	//Special function
	BWAPI::Position firstPosition() const; //Return the position of its first subgoal
};

#endif 