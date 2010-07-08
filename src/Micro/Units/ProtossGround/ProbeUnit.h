#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ProbeUnit : public GroundUnit
{
protected:
    static std::list<BWAPI::UnitType> listPriorite;
public:
    ProbeUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ProbeUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
};