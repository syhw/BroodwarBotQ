#pragma once
#include "Micro/Goals/Goal.h"
#include "BWTA.h"
#include "Micro/Units/BayesianUnit.h"

/*** 
 * A helper/wrapper on top of Goal to facilitate simple attacks (pushs)
 */
class DropGoal: public Goal
{
private:
	void createMidSubgoal();
	BWAPI::Position _dropPos;
	BWTA::BaseLocation* _base;
	pBayesianUnit _dropShipBu;
	pBayesianUnit _reaverBu; // hack
public:
	DropGoal(BWTA::BaseLocation* b, const std::map<BWAPI::UnitType, int>& nU, int priority=96);
	DropGoal(BWAPI::Position p, const std::map<BWAPI::UnitType, int>& nU, int priority=96);
	virtual ~DropGoal();
	void achieve();
	virtual void DropGoal::onOffer(std::set<Unit*> objects);
};
