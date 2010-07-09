#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ScourgeUnit : public FlyingUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    ScourgeUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ScourgeUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};