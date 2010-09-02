#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class CorsairUnit : public FlyingUnit
{
protected:
    static std::set<BWAPI::UnitType> setUnitsPrio;
public:
    CorsairUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~CorsairUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getUnitsPrio();
};