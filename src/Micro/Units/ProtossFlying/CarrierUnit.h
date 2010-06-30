#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class CarrierUnit : public FlyingUnit
{
public:
    CarrierUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~CarrierUnit();
    virtual void micro();
};