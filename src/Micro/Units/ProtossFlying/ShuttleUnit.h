#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ShuttleUnit : public FlyingUnit
{
public:
    ShuttleUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ShuttleUnit();
    virtual void micro();
};