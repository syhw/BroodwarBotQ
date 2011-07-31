#pragma once
#include <BWTA.h>
#include "Micro/Goals/Goal.h"
#include "Micro/Goals/Subgoal.h"
#include "Micro/Goals/SeeSubgoal.h"
#include "Micro/Goals/FindSubgoal.h"
#include "Intelligence/FirstScoutController.h"
#include "Micro/WarManager.h"
class FindEnemyGoal: public Goal
{
public:
	FindEnemyGoal();
	void achieve();
private:
	ScoutController * scoutController;
	WarManager * warManager;
};
