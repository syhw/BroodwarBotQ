#pragma once

#include <GroundUnit.h>
#include <BWAPI.h>

class DragoonUnit : public GroundUnit
{
public:
    DragoonUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~DragoonUnit();
    virtual void micro();
};