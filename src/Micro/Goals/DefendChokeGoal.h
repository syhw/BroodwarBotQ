#pragma once
#include "BWTA.h"
#include "Goal.h"
#include "UnitsGroup.h"

class DefendChokeGoal : public Goal
{
public:
	DefendChokeGoal(UnitsGroup * ug, BWTA::Chokepoint * choke);
	~DefendChokeGoal();
	UnitsGroup * unitsGroup;

};