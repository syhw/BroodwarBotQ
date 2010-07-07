#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class ArchonUnit : public GroundUnit
{
protected:
    static std::list<BWAPI::UnitType> listPriorite;
public:
    ArchonUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~ArchonUnit();
    virtual void micro();
    virtual bool canHit(BWAPI::Unit* enemy);
};