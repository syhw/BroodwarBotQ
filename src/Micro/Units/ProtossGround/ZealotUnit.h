#pragma once

#include "Defines.h"
#include "Micro/Units/GroundUnit.h"
#include <BWAPI.h>

class ZealotUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
    bool decideToFlee();
	void flee();
    void updateTargetEnemy();
    void clearDamages();
    int fightMove();
public:
    ZealotUnit(BWAPI::Unit* u);
    virtual ~ZealotUnit();
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