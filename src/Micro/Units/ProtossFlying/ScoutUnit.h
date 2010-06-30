#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ScoutUnit : public FlyingUnit
{
public:
    ScoutUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ScoutUnit();
    virtual void micro();
};