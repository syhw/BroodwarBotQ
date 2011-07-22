#pragma once

#include "Micro/Units/GroundUnit.h"
#include <BWAPI.h>

class ProbeUnit : public GroundUnit
{
public:
    ProbeUnit(BWAPI::Unit* u,UnitsGroup* ug);
    ~ProbeUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};