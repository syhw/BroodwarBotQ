#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ScourgeUnit : public FlyingUnit
{
protected:
    static std::list<BWAPI::UnitType> listPriorite;
public:
    ScourgeUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ScourgeUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
};