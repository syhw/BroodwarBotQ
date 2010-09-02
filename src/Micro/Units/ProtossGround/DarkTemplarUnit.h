#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class DarkTemplarUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setUnitsPrio;
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    DarkTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DarkTemplarUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getUnitsPrio();
};