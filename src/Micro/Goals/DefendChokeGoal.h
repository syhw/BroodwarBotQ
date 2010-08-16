#pragma once
#include "BWTA.h"
#include "Goal.h"
#include "UnitsGroup.h"

class DefendChokeGoal : public Goal
{
public:
	DefendChokeGoal(UnitsGroup * ug, BWTA::Chokepoint * choke);
	~DefendChokeGoal();
	virtual void checkAchievement();
	virtual void achieve();
private:
	bool enoughUnits();
	BWTA::Chokepoint * choke;
};