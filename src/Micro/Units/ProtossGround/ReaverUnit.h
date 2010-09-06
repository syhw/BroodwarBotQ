#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ReaverUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
public:
    ReaverUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ReaverUnit();
    virtual void micro();
    virtual void check();
    virtual bool inRange(BWAPI::Unit* u);
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};