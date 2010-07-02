#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ScoutUnit : public FlyingUnit
{
protected:
    std::list<BWAPI::UnitType> listPriorite;
public:
    ScoutUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ScoutUnit();
    virtual void micro();
    virtual const std::list<BWAPI::UnitType> getListePrio() const;
    virtual bool canHit(BWAPI::Unit* enemy);
};