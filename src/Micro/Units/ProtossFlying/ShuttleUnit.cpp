#include "ShuttleUnit.h"

ShuttleUnit::ShuttleUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
}

ShuttleUnit::~ShuttleUnit()
{
}

void ShuttleUnit::micro()
{
}

void ShuttleUnit::check()
{
}

int ShuttleUnit::getAttackDuration()
{
    return 0;
}

std::set<BWAPI::UnitType> ShuttleUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}