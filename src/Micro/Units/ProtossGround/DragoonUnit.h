#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class DragoonUnit : public GroundUnit
{
protected:
    static std::set<BWAPI::UnitType> setPrio;
    static int addRange;
    static int attackDuration;
    inline bool decideToFlee();
    inline void simpleFlee();
    int _startedAttack;
public:
    DragoonUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DragoonUnit();
    virtual int addRangeGround();
    virtual int addRangeAir();
    virtual void micro();
    virtual void check();
    static double getMaxRange();
    virtual int getAttackDuration();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};