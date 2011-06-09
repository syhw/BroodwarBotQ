#pragma once

#include "Micro/Units/BayesianUnit.h"

class GroundUnit : public BayesianUnit
{
public:
	GroundUnit(BWAPI::Unit* u, UnitsGroup* ug);
	GroundUnit(BWAPI::Unit* u, UnitsGroup* ug,
		const ProbTables* probtables);
	~GroundUnit();
	virtual void micro() = 0;
	virtual void check() = 0;
	virtual int getAttackDuration() = 0;
	virtual std::set<BWAPI::UnitType> getSetPrio() = 0;
private:
	static ProbTables _sProbTables;
};
