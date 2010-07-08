#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class CorsairUnit : public FlyingUnit
{
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    CorsairUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~CorsairUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual BWAPI::UnitType* getListPriorite();
};