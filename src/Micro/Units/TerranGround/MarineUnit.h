#pragma once

#include "Defines.h"
#include "Micro/Units/GroundUnit.h"
#include <BWAPI.h>

class MarineUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
    bool decideToFlee();
public:
    MarineUnit(BWAPI::Unit* u);
    virtual ~MarineUnit();
#ifdef __LEARNING_PROB_TABLES__
	static void initProbTables();
#endif
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
private:
	static ProbTables _sProbTables;
};