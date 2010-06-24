#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Goal.h"


class DefendGoal: public Goal
{
public:
	//DefendGoal();
	//DefendGoal( const DefendGoal& g);
    DefendGoal(const BWAPI::Position& position);
	virtual ~DefendGoal();

	virtual void achieve(UnitsGroup* ug);
	virtual void checkAchievement(UnitsGroup* ug);
};
