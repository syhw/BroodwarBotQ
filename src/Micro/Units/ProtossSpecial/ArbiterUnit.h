#pragma once

#include <SpecialUnit.h>
#include <BWAPI.h>

class ArbiterUnit : public SpecialUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    ArbiterUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ArbiterUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};