#pragma once

#include "BWAPI.h"
#include "Subgoal.h"

class KillSubgoal : public Subgoal
{
public:
	//TOFINISH
	KillSubgoal(SubgoalLogic l, Unit* u);
	Unit* target;
	bool isRealized();
	void tryToRealize(UnitsGroup * ug);
	double distanceToRealize(UnitsGroup *ug);
protected:
	bool check();
};