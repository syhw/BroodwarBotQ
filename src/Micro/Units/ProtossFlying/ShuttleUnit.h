#pragma once

#include <BWAPI.h>
#include "Micro/Units/FlyingUnit.h"

class ShuttleUnit : public FlyingUnit
{
public:
    ShuttleUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ShuttleUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};