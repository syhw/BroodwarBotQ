#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ShuttleUnit : public FlyingUnit
{
protected:
    static std::list<BWAPI::UnitType> listPriorite;
public:
    ShuttleUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ShuttleUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
};