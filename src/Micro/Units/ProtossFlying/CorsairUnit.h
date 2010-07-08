#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class CorsairUnit : public FlyingUnit
{
protected:
    static std::list<BWAPI::UnitType> listPriorite;
public:
    CorsairUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~CorsairUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
};