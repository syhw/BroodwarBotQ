#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ArchonUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setUnitsPrio;
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
    ArchonUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ArchonUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getUnitsPrio();
};