#pragma once
#include "Micro/Goals/Goal.h"
#include "Utils/CSingleton.h"
#include <stdlib.h>
#include "Micro/UnitsGroup.h"
#include <BWAPI.h>
#include "Micro/WarManager.h"
#include "../Formations/Formation.h"

class GoalManager : public CSingleton<GoalManager>
{
	friend class CSingleton<GoalManager>;
private:
	std::list<pGoal> goals;
	GoalManager();
	~GoalManager();
	std::map<BWAPI::Unit*, pBayesianUnit> militaryUnits;
	std::list<BWAPI::Unit*> inTrainingUnits;
public:
	void update();
	void addGoal(pGoal g);

};