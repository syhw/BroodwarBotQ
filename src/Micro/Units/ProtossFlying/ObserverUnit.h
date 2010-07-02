#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class ObserverUnit : public FlyingUnit
{
protected:
    std::list<BWAPI::UnitType> listPriorite;
public:
    ObserverUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ObserverUnit();
    virtual void micro();
    virtual const std::list<BWAPI::UnitType> getListePrio() const;
    virtual bool canHit(BWAPI::Unit* enemy);
};