#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include "Micro/Goals/Goal.h"
#include "SeeSubgoal.h"
#include "Micro/UnitsGroup.h"

class ExploreGoal: public Goal
{
public:
	ExploreGoal::ExploreGoal(BWAPI::TilePosition tp);
	ExploreGoal(BWTA::Region* region);
	void achieve();
};
