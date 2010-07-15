#pragma once
#include <BWAPI.h>
#include "Subgoal.h"
#include "Regions.h"

class FindSubgoal : public Subgoal
{

public:
	FindSubgoal(SubgoalLogic l);
	virtual bool isRealized();
	virtual void tryToRealize(UnitsGroup *ug);
	virtual double distanceToRealize(UnitsGroup *ug);
protected:
	virtual bool check();
	bool achieved;
	Regions * regions;
};