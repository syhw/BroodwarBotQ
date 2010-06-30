#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ProbeUnit : public GroundUnit
{
public:
    ProbeUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ProbeUnit();
    virtual void micro();
};