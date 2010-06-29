#pragma once
#include "CSingleton.h"
#include <stdlib.h>
#include "Goal.h"
#include "UnitsGroup.h"
#include "ScoutManager.h"
#include <BWAPI.h>
#include "MicroManager.h"
#include "../Formations/Formation.h"
class GoalManager : public CSingleton<GoalManager>
{
	friend class CSingleton<GoalManager>;
private:
	MicroManager * microManager;
	ScoutManager * scoutManager;
	GoalManager();
	~GoalManager();

public:
	virtual void onFrame();
	void findUnit(pGoal goal);
};