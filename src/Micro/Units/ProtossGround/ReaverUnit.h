#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ReaverUnit : public GroundUnit
{
public:
    ReaverUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ReaverUnit();
    virtual void micro();
};