#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class MutaliskUnit : public FlyingUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    MutaliskUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~MutaliskUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};