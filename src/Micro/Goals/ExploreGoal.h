#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include "Micro/Goals/Goal.h"
#include "SeeSubgoal.h"
#include "Micro/UnitsGroup.h"

class ExploreGoal: public Goal
{
	inline void needAScoutingUnit();
public:
	ExploreGoal::ExploreGoal(BWAPI::TilePosition tp, int priority = 50);
	ExploreGoal(BWTA::Region* region, int priority = 50);
	void achieve();
};
