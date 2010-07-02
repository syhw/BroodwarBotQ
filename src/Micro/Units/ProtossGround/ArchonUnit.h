#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ArchonUnit : public GroundUnit
{
protected:
    std::list<BWAPI::UnitType> listPriorite;
public:
    ArchonUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ArchonUnit();

    virtual void micro();
    virtual const std::list<BWAPI::UnitType> getListePrio() const;
};