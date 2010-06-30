#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ObserverUnit : public FlyingUnit
{
public:
    ObserverUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ObserverUnit();
    virtual void micro();
};