#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class CarrierUnit : public FlyingUnit
{
public:
    static std::list<BWAPI::UnitType> listPriorite;
    CarrierUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~CarrierUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
};