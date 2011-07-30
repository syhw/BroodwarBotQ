#pragma once

#include "Defines.h"
#include "Micro/Units/GroundUnit.h"
#include <BWAPI.h>

class MedicUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
    bool decideToFlee();
    int fightMove();
public:
    MedicUnit(BWAPI::Unit* u);
    ~MedicUnit();
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