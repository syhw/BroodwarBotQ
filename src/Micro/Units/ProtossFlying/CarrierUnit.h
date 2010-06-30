#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class CarrierUnit : public FlyingUnit
{
protected:
    std::list<BWAPI::UnitType> listPriorite;
public:
    CarrierUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~CarrierUnit();
    virtual void micro();
    virtual const std::list<BWAPI::UnitType> getListePrio() const;
};