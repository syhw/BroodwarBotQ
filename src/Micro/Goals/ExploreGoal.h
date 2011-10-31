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
	int _firstRealized;
public:
	ExploreGoal(BWTA::Region* region, int priority = 50);
	virtual ~ExploreGoal();
	void achieve();
	virtual void onOffer(std::set<BWAPI::Unit*> objects);
};
