#pragma once
#include "Micro/Goals/Goal.h"

// #define __MID_SUBGOALS__

/*** 
 * A helper/wrapper on top of Goal to facilitate simple attacks (pushs)
 * With abort() if our force ratio is < 0.75
 */
class AttackGoal: public Goal
{
private:
#ifdef __MID_SUBGOALS__
	void createMidSubgoal();
#endif
public:
	AttackGoal(const std::map<BWAPI::UnitType, int>& miniUnits, 
		BWAPI::Position p, int priority = 50, int firstFrame = 0);
	AttackGoal(BWAPI::Position p, int priority = 50, int firstFrame = 0);
	AttackGoal(pSubgoal subgoal, int priority = 50, int firstFrame = 0);
	virtual ~AttackGoal();
	void achieve();
	std::string getName();
	std::string getShortName();
	void abort();
	//virtual void onOffer(std::set<BWAPI::Unit*> objects);
};
