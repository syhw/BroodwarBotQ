#pragma once
#include "CSingleton.h"
#include <stdlib.h>
#include "Goal.h"
#include "UnitsGroup.h"
#include <BWAPI.h>
#include "MicroManager.h"
#include "../Formations/Formation.h"


class GoalManager : public CSingleton<GoalManager>
{
	friend class CSingleton<GoalManager>;
private:
	MicroManager * microManager;
	std::map<UnitsGroup *, std::list<pGoal>> attributedGoals;
	Regions * regions;

public:
	GoalManager();
	~GoalManager();
	void newGoal(pGoal p);
	void insert(UnitsGroup * ug, pGoal g);
	bool remove(UnitsGroup * ug, pGoal g);//Return true if the deletion is accomplished, false in the other case
	bool clean(UnitsGroup * ug); //Remove the pair that uses this ug as key in attributedGoals
	void findUnitsGroup(pGoal goal);
	void goalDone(UnitsGroup * ug, pGoal p);//This method will decide what to do next with the unitgroup (delete, merge, new goal...)
};