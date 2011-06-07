#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossSpecial/DarkArchonUnit.h"

DarkArchonUnit::DarkArchonUnit(BWAPI::Unit* u, BasicUnitsGroup* ug)
: SpecialUnit(u, ug)
{
}

DarkArchonUnit::~DarkArchonUnit()
{
}

void DarkArchonUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkArchonUnit::micro non implémenté !");
#endif
}

void DarkArchonUnit::check()
{
}

int DarkArchonUnit::getAttackDuration()
{
    return 3;
}

std::set<BWAPI::UnitType> DarkArchonUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}