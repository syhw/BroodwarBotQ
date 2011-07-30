#pragma once

#include <BWAPI.h>
#include "Micro/Units/FlyingUnit.h"

class ObserverUnit : public FlyingUnit
{
    EUnitsFilter* _eUnitsFilter;
public:
    ObserverUnit(BWAPI::Unit* u);
    ~ObserverUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};