#pragma once

#include <SpecialUnit.h>
#include <BWAPI.h>

class DarkArchonUnit : public SpecialUnit
{
public:
    DarkArchonUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DarkArchonUnit();
    virtual void micro();
};