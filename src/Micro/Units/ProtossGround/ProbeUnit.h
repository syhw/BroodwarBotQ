#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ProbeUnit : public GroundUnit
{
protected:
    std::list<BWAPI::UnitType> listPriorite;
public:
    ProbeUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ProbeUnit();
    virtual void micro();
    virtual const std::list<BWAPI::UnitType> getListePrio() const;
};