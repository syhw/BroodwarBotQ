#pragma once

#include <SpecialUnit.h>
#include <BWAPI.h>

class HighTemplarUnit : public SpecialUnit
{
public:
    static BWAPI::UnitType setPriorite[NUMBER_OF_PRIORITY];
    HighTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~HighTemplarUnit();
    std::pair<BWAPI::Position, int> bestStormPos();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};