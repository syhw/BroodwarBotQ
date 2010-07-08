#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ScoutUnit : public FlyingUnit
{
protected:
    static std::list<BWAPI::UnitType> listPriorite;
public:
    ScoutUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ScoutUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
};