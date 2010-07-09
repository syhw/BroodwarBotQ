#pragma once

#include <SpecialUnit.h>
#include <BWAPI.h>

class DarkArchonUnit : public SpecialUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    DarkArchonUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DarkArchonUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};