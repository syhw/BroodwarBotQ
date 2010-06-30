#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ZealotUnit : public GroundUnit
{
public:
    ZealotUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ZealotUnit();
    virtual void micro();
};