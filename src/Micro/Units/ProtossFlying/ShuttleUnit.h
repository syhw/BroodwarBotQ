#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ShuttleUnit : public FlyingUnit
{
protected:
    std::list<BWAPI::UnitType> listPriorite;
public:
    ShuttleUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ShuttleUnit();
    virtual void micro();
    virtual const std::list<BWAPI::UnitType> getListePrio() const;   
    virtual bool canHit(BWAPI::Unit* enemy);
};