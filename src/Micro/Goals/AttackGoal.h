#pragma once
#include "Micro/Goals/Goal.h"

/*** 
 * A helper/wrapper on top of Goal to facilitate simple attacks (pushs)
 */
class AttackGoal: public Goal
{
private:
	void createMidSubgoal();
public:
	AttackGoal(const std::map<BWAPI::UnitType, int>& miniUnits, BWAPI::Position p);
	void achieve();
};
