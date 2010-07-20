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
	AttackGoal(BWAPI::Position pos);
	AttackGoal( const AttackGoal& g);

	virtual ~AttackGoal();
	BWAPI::Position pos;
};
