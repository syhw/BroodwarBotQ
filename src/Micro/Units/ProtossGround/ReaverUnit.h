#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ReaverUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
public:
    static BWAPI::UnitType setPriorite[NUMBER_OF_PRIORITY];
    ReaverUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ReaverUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};