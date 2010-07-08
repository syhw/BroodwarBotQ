#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class DarkTemplarUnit : public GroundUnit
{
protected:
    static std::list<BWAPI::UnitType> listPriorite;
public:
    DarkTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DarkTemplarUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
};