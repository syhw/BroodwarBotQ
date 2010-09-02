#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ScoutUnit : public FlyingUnit
{
protected:
    static std::set<BWAPI::UnitType> setUnitsPrio;
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    ScoutUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ScoutUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getUnitsPrio();
};