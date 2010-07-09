#pragma once

#include <SpecialUnit.h>
#include <BWAPI.h>

class HighTemplarUnit : public SpecialUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    HighTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~HighTemplarUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};