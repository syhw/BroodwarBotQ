#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ZealotUnit : public GroundUnit
{
protected:
    std::list<BWAPI::UnitType> listPriorite;
public:
    ZealotUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ZealotUnit();
    virtual void micro();
    virtual const std::list<BWAPI::UnitType> getListePrio() const;
};