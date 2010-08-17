#pragma once
#include "BWTA.h"
#include "BWAPI.h"
#include "Goal.h"
#include "UnitsGroup.h"
#include "Base.h"

class DefendBaseGroundGoal : public Goal
{
public:
	DefendBaseGroundGoal(UnitsGroup * ug,  Base * b);
	~DefendBaseGroundGoal();
	virtual void checkAchievement();
	virtual void achieve();
private:
	bool enoughUnits();
	BWTA::Chokepoint * choke;
};