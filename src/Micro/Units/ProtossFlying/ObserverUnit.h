#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ObserverUnit : public FlyingUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    ObserverUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ObserverUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};