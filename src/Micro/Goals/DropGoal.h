#pragma once
#include "Micro/Goals/Goal.h"
#include "Micro/UnitsGroup.h"
#include "BWTA.h"
#include "Micro/Units/BayesianUnit.h"


class DropGoal: public Goal
{
private:
	void createMidSubgoal();
	BWAPI::Position _dropPos;
	BWTA::BaseLocation* _base;
	pBayesianUnit _dropShipBu;
	std::list<pBayesianUnit> _dropeeBu;
public:
	DropGoal(BWTA::BaseLocation* b, const std::map<BWAPI::UnitType, int>& nU, int priority=96);
	DropGoal(BWAPI::Position p, const std::map<BWAPI::UnitType, int>& nU, int priority=96);
	virtual ~DropGoal();
	void achieve();
	virtual void update();
	virtual void onOffer(std::set<BWAPI::Unit*> objects);
};
