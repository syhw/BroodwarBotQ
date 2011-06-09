#pragma once

#include "Micro/Units/GroundUnit.h"
#include <BWAPI.h>

class DarkTemplarUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
public:
    DarkTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DarkTemplarUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};