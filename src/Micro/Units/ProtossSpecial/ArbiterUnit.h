#pragma once

#include "Micro/Units/FlyingUnit.h"
#include <BWAPI.h>

class ArbiterUnit : public FlyingUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
public:
    ArbiterUnit(BWAPI::Unit* u, BasicUnitsGroup* ug);
    ~ArbiterUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};