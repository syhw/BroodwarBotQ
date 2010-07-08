#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ScoutUnit : public FlyingUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    ScoutUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ScoutUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};