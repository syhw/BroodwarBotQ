#pragma once
#include <BWTA.h>
#include "Goal.h"
#include "Subgoal.h"
#include "SeeSubgoal.h"
#include "FindSubgoal.h"
#include "ScoutController.h"
#include "WarManager.h"
class FindEnemyGoal: public Goal
{
public:
	FindEnemyGoal();
	void achieve();
private:
	ScoutController * scoutController;
	WarManager * warManager;
};
