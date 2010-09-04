#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class MutaliskUnit : public FlyingUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
public:
    MutaliskUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~MutaliskUnit();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};