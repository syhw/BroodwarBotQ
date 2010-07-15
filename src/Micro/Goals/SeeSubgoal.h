#pragma once
#include <BWAPI.h>
#include "Subgoal.h"

class SeeSubgoal : public Subgoal
{

public:
	SeeSubgoal(SubgoalLogic l, BWAPI::Position pos);
	bool isRealized();
	void tryToRealize(UnitsGroup *ug);
	double distanceToRealize(UnitsGroup *ug);
protected:
	bool check();
	BWAPI::Position pos;
	bool achieved;
};