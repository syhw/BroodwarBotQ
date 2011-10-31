#pragma once
#include "Micro/Goals/Goal.h"
#include "Utils/CSingleton.h"
#include <stdlib.h>
#include "Micro/UnitsGroup.h"
#include <BWAPI.h>
#include "Macro/Arbitrator.h"

/***
 * A singleton which tracks count of created (in training / completed)
 * units + maintain a list of goals it dispatches update() to
 */
class GoalManager : public CSingleton<GoalManager>
{
	friend class CSingleton<GoalManager>;
private:
	bool _firstPoke; // hack, remove TODO
	std::list<pGoal> _goals;
	GoalManager();
	~GoalManager();
	std::map<BWAPI::Unit*, pBayesianUnit> _completedUnits;
	std::list<BWAPI::Unit*> _inTrainingUnits;
public:
	int attackGoals;
	void addGoal(pGoal g);
	void addGoals(const std::list<pGoal>& l);
	void onUnitCreate(BWAPI::Unit* u);
	void onUnitDestroy(BWAPI::Unit* u);
	void onUnitRenegade(BWAPI::Unit* u);
	const std::map<BWAPI::Unit*, pBayesianUnit>& getCompletedUnits() const;
	pBayesianUnit getCompletedUnit(BWAPI::Unit* u);

	virtual void update();
};