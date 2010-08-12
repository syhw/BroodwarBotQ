#pragma once
#include "Goal.h"

class AttackGoal: public Goal
{
public:
    AttackGoal(UnitsGroup* ug, BWAPI::Position p);
};
