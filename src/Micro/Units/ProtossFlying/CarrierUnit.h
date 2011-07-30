#pragma once

#include <BWAPI.h>
#include "Micro/Units/FlyingUnit.h"

class CarrierUnit : public FlyingUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
public:
    CarrierUnit(BWAPI::Unit* u);
    ~CarrierUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};