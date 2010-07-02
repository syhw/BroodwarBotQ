#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class DarkTemplarUnit : public GroundUnit
{
protected:
    std::list<BWAPI::UnitType> listPriorite;
public:
    DarkTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DarkTemplarUnit();
    virtual void micro();
    virtual const std::list<BWAPI::UnitType> getListePrio() const;
};