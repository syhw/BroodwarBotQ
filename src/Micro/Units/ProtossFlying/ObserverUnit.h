#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ObserverUnit : public FlyingUnit
{
    EUnitsFilter* _eUnitsFilter;
public:
    ObserverUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ObserverUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};