#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "Formations.h"
#include "Subgoal.h"
#ifndef _SMART_POINTER_GOAL_INCLUDED
#define _SMART_POINTER_GOAL_INCLUDED
#include <boost/shared_ptr.hpp>
class Goal;
typedef boost::shared_ptr<Goal> pGoal;
#endif

class UnitsGroup;
typedef enum
{
	SCOUT = 0,
	ATTACK = 1,
	DEFEND = 2
} GoalType;

typedef enum
{
	GS_ND_STATUS           = 0,
	GS_ACHIEVED            = 0,
	GS_IN_PROGRESS         = 1,
	GS_MOVE_TO             = 2,
	GS_FLEE                = 3
} GoalStatus;


class Goal
{
protected:
	unsigned int achiviedCpt;
	std::list<pSubgoal> subgoals;

public:
	Goal();
	Goal(const Goal& g);
	GoalType type;          /**< type of the goal */
	GoalStatus status;      /**< status of the goal */
	pFormation formation;   /**< formation to adopt during the goal */
	std::string purpose;    /**< string describing the purpose of the goal */

	Goal(const char* text, pFormation f);
	virtual ~Goal();

	virtual void achieve(UnitsGroup* ug);
	virtual void checkAchievement(UnitsGroup* ug);
    virtual std::string getPurpose() const;

	void addSubgoal(pSubgoal s);
};