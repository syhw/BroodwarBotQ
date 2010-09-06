#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ScourgeUnit : public FlyingUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
public:
    ScourgeUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ScourgeUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};