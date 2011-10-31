#pragma once
#include "Micro/Goals/Goal.h"

class AttackGoal: public Goal
{
private:
	void createMidSubgoal();
public:
    AttackGoal(UnitsGroup* ug, BWAPI::Position p);
	AttackGoal(UnitsGroup* ug, BWAPI::TilePosition tp);
	void achieve();
};
