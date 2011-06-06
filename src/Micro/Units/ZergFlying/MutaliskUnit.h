#pragma once

#include <BWAPI.h>
#include "Micro/Units/FlyingUnit.h"

class MutaliskUnit : public FlyingUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
    inline bool decideToFlee();
public:
    MutaliskUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~MutaliskUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};