#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class DarkTemplarUnit : public GroundUnit
{
public:
    DarkTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DarkTemplarUnit();
    virtual void micro();
};