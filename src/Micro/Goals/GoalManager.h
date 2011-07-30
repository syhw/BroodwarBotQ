#pragma once
#include "Micro/Goals/Goal.h"
#include "Utils/CSingleton.h"
#include <stdlib.h>
#include "Micro/UnitsGroup.h"
#include <BWAPI.h>
#include "Micro/WarManager.h"
#include "../Formations/Formation.h"

/***
 * A singleton which tracks count of created (in training / completed)
 * units + maintain a list of goals it dispatches update() to
 */
class GoalManager : public CSingleton<GoalManager>
{
	friend class CSingleton<GoalManager>;
private:
	std::list<pGoal> _goals;
	GoalManager();
	~GoalManager();
	std::map<BWAPI::Unit*, pBayesianUnit> _completedUnits;
	std::list<BWAPI::Unit*> _inTrainingUnits;
public:
	void update();
	void addGoal(pGoal g);
	void onUnitCreate(BWAPI::Unit* u);
	void onUnitDestroy(BWAPI::Unit* u);
	const std::map<BWAPI::Unit*, pBayesianUnit>& getCompletedUnits() const;
};