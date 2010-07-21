#pragma once
#include <BWTA.h>
#include "Goal.h"
#include "Regions.h"
#include "Subgoal.h"
#include "SeeSubgoal.h"
#include "FindSubgoal.h"
#include "ScoutManager.h"
#include "MicroManager.h"
class FindEnemyGoal: public Goal
{
public:
	FindEnemyGoal();
	void achieve();
private:
	ScoutManager * scoutManager;
	MicroManager * microManager;
};
