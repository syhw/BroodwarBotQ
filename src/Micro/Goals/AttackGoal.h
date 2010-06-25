#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Goal.h"


class AttackGoal: public Goal
{
protected:
    BWAPI::Unit* unit;

public:
	AttackGoal();
	AttackGoal( const AttackGoal& g);
    AttackGoal(const BWAPI::Position& position, BWAPI::Unit* unit);
	virtual ~AttackGoal();

	virtual void achieve(UnitsGroup* ug);
	virtual void checkAchievement(UnitsGroup* ug);
};
