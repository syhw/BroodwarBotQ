#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class MutaliskUnit : public FlyingUnit
{
protected:
    static std::list<BWAPI::UnitType> listPriorite;
public:
    MutaliskUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~MutaliskUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
};