#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ShuttleUnit : public FlyingUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    ShuttleUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ShuttleUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};