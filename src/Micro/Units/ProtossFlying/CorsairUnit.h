#pragma once

#include <BWAPI.h>
#include <FlyingUnit.h>

class CorsairUnit : public FlyingUnit
{
public:
    CorsairUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~CorsairUnit();
    virtual void micro();
};