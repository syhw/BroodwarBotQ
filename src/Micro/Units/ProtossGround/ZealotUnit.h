#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ZealotUnit : public GroundUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    ZealotUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ZealotUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};