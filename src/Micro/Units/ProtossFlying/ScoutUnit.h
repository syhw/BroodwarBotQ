#pragma once

#include <BWAPI.h>
#include "Micro/Units/FlyingUnit.h"

class ScoutUnit : public FlyingUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
    inline bool decideToFlee();
public:
    ScoutUnit(BWAPI::Unit* u
    ~ScoutUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};