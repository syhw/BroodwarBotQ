#pragma once
#include "Goal.h"
#include "CSingleton.h"
#include <stdlib.h>
#include "UnitsGroup.h"
#include <BWAPI.h>
#include "MicroManager.h"
#include "../Formations/Formation.h"

//TOCHANGE AT least the class name

class GoalManager : public CSingleton<GoalManager>
{
	friend class CSingleton<GoalManager>;
private:
	MicroManager * microManager;
	std::map<UnitsGroup *, std::list<pGoal> > attributedGoals;
	Regions * regions;

public:
	GoalManager();
	~GoalManager();
	void setDependencies(MicroManager * micro, Regions * reg);
	void insert(UnitsGroup * ug, pGoal g);
	bool remove(UnitsGroup * ug, pGoal g);//Return true if the deletion is accomplished, false in the other case
	bool clean(UnitsGroup * ug); //Remove the pair that uses this ug as key in attributedGoals
};