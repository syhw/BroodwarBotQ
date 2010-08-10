#pragma once
#include "BWTA.h"
#include "Goal.h"
#include "UnitsGroup.h"

class DefendChokeGoal : public Goal
{
public:
	DefendChokeGoal(UnitsGroup * ug, BWTA::Chokepoint * choke); //This goal must have max 1 subgoal : a formation subgoal
	~DefendChokeGoal();
	virtual void checkAchievement();
	virtual void achieve();
private:
	bool enoughUnits();
};