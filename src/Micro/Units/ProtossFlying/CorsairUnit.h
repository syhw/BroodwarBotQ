#pragma once

#include <BWAPI.h>
#include "Micro/Units/FlyingUnit.h"

class CorsairUnit : public FlyingUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
    inline bool decideToFlee();
public:
    CorsairUnit(BWAPI::Unit* u, BasicUnitsGroup* ug);
    ~CorsairUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};