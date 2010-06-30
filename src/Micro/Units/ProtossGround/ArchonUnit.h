#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ArchonUnit : public GroundUnit
{
public:
    ArchonUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ArchonUnit();

    virtual void micro();
};