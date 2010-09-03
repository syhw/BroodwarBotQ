#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ObserverUnit : public FlyingUnit
{
public:
    ObserverUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ObserverUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};