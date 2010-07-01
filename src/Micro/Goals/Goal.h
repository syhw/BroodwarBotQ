#ifndef GOAL__H
#define GOAL__H
#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "Formations.h"
#include "Subgoal.h"
#include <boost/shared_ptr.hpp>
#include "ScoutManager.h"
class ScoutManager;
class Goal;
typedef boost::shared_ptr<Goal> pGoal;


class UnitsGroup;
typedef enum
{
	UNDEFINED = 0,
	SCOUT = 1,
	ATTACK = 2,
	DEFEND = 3
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

typedef enum
{	
	GP_UNDEFINED  = 0,
	GP_FINDENEMY = 1,
	GP_CHECK_XP = 2

}GoalPurpose;

class Goal
{
	//A goal contain a list of subgoals
	//Some subgoals must be validated only once (SC_ACTIVE),
	//other must be validated all along the accomplishment (SC_ONCE).
protected:
	ScoutManager * scoutManager;
	std::list<pSubgoal> subgoals; //The subgoals cannot be shared
	GoalType type;          /**< type of the goal */
	GoalStatus status;      /**< status of the goal */
	pFormation formation;    /**< formation to accomplish the goal */
	GoalPurpose purpose;    /**< The purpose is an alternative way to accomplish the goal */

public:

	//Constructors
	Goal(GoalType t); //Don't forgot to set a formation and movement if wanted
	//Else avoid enemy and void formation is used
	Goal(GoalType t, GoalPurpose p);
	virtual ~Goal();
	
	virtual void achieve(UnitsGroup* ug);//Start the goal

	virtual void checkAchievement(UnitsGroup* ug);//Check if accomplished
	//(Check if all subgoals are accomplished)

    virtual GoalPurpose getPurpose() const;

	//Mutators
	void addSubgoal(pSubgoal s);
	void setFormation(pFormation f); //Warning, the formation is not duplicated.

	//Accessors
	pFormation getFormation() const;
	GoalStatus getStatus() const;
	GoalType getType() const;
};

#endif 