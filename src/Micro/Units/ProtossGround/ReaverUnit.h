#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ReaverUnit : public GroundUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    ReaverUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ReaverUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};