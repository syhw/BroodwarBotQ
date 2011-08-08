#pragma once
#include "Micro/Goals/Goal.h"

/*** 
 * A helper/wrapper on top of Goal to facilitate simple attacks (pushs)
 */
class RegroupGoal: public Goal
{
private:
	void createMidSubgoal();
public:
	RegroupGoal(BWAPI::Position p, int priority = 10, int firstFrame = 0);
	virtual ~RegroupGoal();
	virtual void RegroupGoal::achieve();
};
