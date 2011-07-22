#pragma once

#include <BWAPI.h>
#include "Micro/Units/FlyingUnit.h"

class ScourgeUnit : public FlyingUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
    static std::set<BWAPI::Unit*> alreadyTargeted; // hack only for the tournament 1
public:
    ScourgeUnit(BWAPI::Unit* u,UnitsGroup* ug);
    ~ScourgeUnit();
    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};