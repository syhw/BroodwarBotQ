#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Goal.h"
#include "Subgoal.h"

class AttackGoal: public Goal
{
protected:
    BWAPI::Unit* unit;

public:
	AttackGoal();
	AttackGoal(BWAPI::Position p);
	AttackGoal( const AttackGoal& g);

	virtual ~AttackGoal();
	virtual void checkAchievement(UnitsGroup* ug);
};
