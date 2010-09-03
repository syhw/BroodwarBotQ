#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class CarrierUnit : public FlyingUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
public:
    CarrierUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~CarrierUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};