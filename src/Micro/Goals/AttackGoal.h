#pragma once
#include "Micro/Goals/Goal.h"

/*** 
 * A helper/wrapper on top of Goal to facilitate simple attacks (pushs)
 */
class AttackGoal: public Goal
{
private:
	void createMidSubgoal();
public:
	AttackGoal(const std::map<BWAPI::UnitType, int>& miniUnits, 
		BWAPI::Position p, int priority = 50, int firstFrame = 0);
	AttackGoal(BWAPI::Position p, int priority = 50, int firstFrame = 0);
	AttackGoal(pSubgoal subgoal, int priority = 50, int firstFrame = 0);
	void achieve();
	void canBidOn(BWAPI::Unit* u);
	//virtual void onOffer(std::set<BWAPI::Unit*> objects);
};
