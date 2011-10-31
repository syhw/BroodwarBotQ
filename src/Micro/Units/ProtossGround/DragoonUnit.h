#pragma once

#include "Micro/Units/GroundUnit.h"
#include <BWAPI.h>

class DragoonUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
    static int addRange;
    static int attackDuration;
    virtual bool decideToFlee();
public:
    DragoonUnit(BWAPI::Unit* u);
    virtual ~DragoonUnit();
    virtual int addRangeGround();
    virtual int addRangeAir();
    static double getMaxRange();

    virtual void micro();
    virtual void check();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};