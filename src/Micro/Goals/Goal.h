#pragma once
#include <boost/shared_ptr.hpp>
class Goal;
typedef boost::shared_ptr<Goal> pGoal;
#include "Subgoal.h"
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "Formations.h"


class UnitsGroup;

typedef enum
{
	GS_ND_STATUS           = 0,
	GS_ACHIEVED            = 0,
	GS_IN_PROGRESS         = 1,
	GS_FLEE                = 2,
	GS_NOT_STARTED         = 3
} GoalStatus;

class Goal
{
	//A goal contain a list of subgoals
	//Some subgoals must be validated only once (SC_ACTIVE),
	//other must be validated all along the accomplishment (SC_ONCE).
protected:
	std::list<pSubgoal> subgoals; //The subgoals cannot be shared
	GoalStatus status;      /**< status of the goal */
public:

	//Constructors
	Goal();
	virtual ~Goal();
	
	virtual void achieve(UnitsGroup* ug);//Start the goal

	virtual void checkAchievement(UnitsGroup* ug);//Check if accomplished
	//(Check if all subgoals are accomplished)

	//Mutators
	void addSubgoal(pSubgoal s);
	void setStatus(GoalStatus s);

	//Accessors
	GoalStatus getStatus() const;
};