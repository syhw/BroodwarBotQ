#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include "Goal.h"
#include "Regions.h"
#include "SeeSubgoal.h"
#include "UnitsGroup.h"
class ExploreGoal: public Goal
{
	ExploreGoal(UnitsGroup* ug,BWTA::Region* region);
	void achieve();
};
