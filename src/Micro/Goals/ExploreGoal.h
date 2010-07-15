#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include "Goal.h"
#include "Regions.h"
#include "SeeSubgoal.h"
class ExploreGoal: public Goal
{
	ExploreGoal(BWTA::Region* region);
	void achieve(UnitsGroup * ug);
};
