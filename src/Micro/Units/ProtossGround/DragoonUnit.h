#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class DragoonUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
    static int addRange;
public:
    DragoonUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DragoonUnit();
    virtual int addRangeGround();
    virtual int addRangeAir();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    static double getMaxRange();
    virtual bool withinRange(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};