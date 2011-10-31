#pragma once

#include "Micro/Units/BayesianUnit.h"

class SpecialUnit : public BayesianUnit
{
public:
	SpecialUnit(BWAPI::Unit* u);
	virtual ~SpecialUnit();
	virtual void micro() = 0;
	virtual void check() = 0;
	virtual int getAttackDuration();
	virtual std::set<BWAPI::UnitType> getSetPrio() = 0;
private:
	static ProbTables _sProbTables;
};