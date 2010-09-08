#pragma once
#include <BWTA.h>
#include "Goal.h"
#include "Subgoal.h"
#include "SeeSubgoal.h"
#include "FindSubgoal.h"
#include "ScoutManager.h"
#include "WarManager.h"
class FindEnemyGoal: public Goal
{
public:
	FindEnemyGoal();
	void achieve();
private:
	ScoutManager * scoutManager;
	WarManager * warManager;
};
