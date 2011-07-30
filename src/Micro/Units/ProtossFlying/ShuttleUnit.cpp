#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossFlying/ShuttleUnit.h"

ShuttleUnit::ShuttleUnit(BWAPI::Unit* u)
: FlyingUnit(u)
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