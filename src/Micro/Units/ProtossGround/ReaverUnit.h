#pragma once

#include "Micro/Units/GroundUnit.h"
#include <BWAPI.h>

class ReaverUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
public:
    ReaverUnit(BWAPI::Unit* u);
    ~ReaverUnit();
    virtual void micro();
    virtual void check();
    virtual bool inRange(BWAPI::Unit* u);
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};