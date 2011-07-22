#pragma once

#include "Micro/Units/SpecialUnit.h"
#include <BWAPI.h>

class DarkArchonUnit : public SpecialUnit
{
public:
    DarkArchonUnit(BWAPI::Unit* u,UnitsGroup* ug);
    ~DarkArchonUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};