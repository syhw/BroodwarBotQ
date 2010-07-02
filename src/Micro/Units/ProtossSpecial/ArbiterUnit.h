#pragma once

#include <SpecialUnit.h>
#include <BWAPI.h>

class ArbiterUnit : public SpecialUnit
{
public:
    ArbiterUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ArbiterUnit();
    virtual void micro();
};