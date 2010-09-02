#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ProbeUnit : public GroundUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    ProbeUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ProbeUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};