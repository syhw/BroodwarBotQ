#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ShuttleUnit : public FlyingUnit
{
public:
    ShuttleUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ShuttleUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};