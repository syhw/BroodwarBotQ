#pragma once
#include "Goal.h"

class AttackGoal: public Goal
{
private:
	void createMidSubgoal();
public:
    AttackGoal(UnitsGroup* ug, BWAPI::Position p);
	void achieve();
};
