#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ReaverUnit : public GroundUnit
{
protected:
    static std::list<BWAPI::UnitType> listPriorite;
public:
    ReaverUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ReaverUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
};