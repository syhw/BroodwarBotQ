#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class DarkTemplarUnit : public GroundUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    DarkTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DarkTemplarUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};