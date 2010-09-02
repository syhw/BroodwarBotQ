#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class DragoonUnit : public GroundUnit
{
protected:
    static int addRange;
public:
    static BWAPI::UnitType listPriorite[NUMBER_OF_PRIORITY];
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
    virtual BWAPI::UnitType* getListPriorite();
};