#pragma once
#include "Micro/Goals/Goal.h"
#include "BWTA.h"

/*** 
 * A helper/wrapper on top of Goal to facilitate simple attacks (pushs)
 */
class DropGoal: public Goal
{
private:
	void createMidSubgoal();
	BWAPI::Position _dropPos;
public:
	DropGoal(BWAPI::Position p, const std::map<BWAPI::UnitType, int>& nU, int priority);
	void achieve();
};
