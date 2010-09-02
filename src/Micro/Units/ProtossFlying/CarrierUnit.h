#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class CarrierUnit : public FlyingUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    CarrierUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~CarrierUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};