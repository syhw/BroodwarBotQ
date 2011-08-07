#pragma once
#include <BWTA.h>
#include <BWAPI.h>
#include "Micro/Goals/Goal.h"
#include "SeeSubgoal.h"
#include "Micro/UnitsGroup.h"

class ExploreGoal: public Goal
{
	inline void needAScoutingUnit();
	BWTA::Region* _region;
public:
	ExploreGoal(BWTA::Region* region, int priority = 50);
	virtual ~ExploreGoal();
	void achieve();
};
