#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include "Goal.h"
#include "Regions.h"
#include "SeeSubgoal.h"
#include "UnitsGroup.h"
class ExploreGoal: public Goal
{
public:
	ExploreGoal(BWTA::Region* region);
	void achieve();

};
