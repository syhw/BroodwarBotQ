#pragma once
#include "Micro/Goals/Subgoal.h"

class KillSubgoal : public Subgoal
{
public:
	KillSubgoal(SubgoalLogic l, UnitsGroup* ug, BWAPI::Unit* u);
	BWAPI::Unit* _target;
	bool isRealized();
	void tryToRealize();
	double distanceToRealize();
};