#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class CorsairUnit : public FlyingUnit
{
protected:
    std::list<BWAPI::UnitType> listPriorite;
public:
    CorsairUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~CorsairUnit();
    virtual void micro();
    virtual const std::list<BWAPI::UnitType> getListePrio() const;
};