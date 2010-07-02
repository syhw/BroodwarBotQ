#pragma once

#include <SpecialUnit.h>
#include <BWAPI.h>

class HighTemplarUnit : public SpecialUnit
{
public:
    HighTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~HighTemplarUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
};