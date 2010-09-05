#include "MutaliskUnit.h"

std::set<BWAPI::UnitType> MutaliskUnit::setPrio;

MutaliskUnit::MutaliskUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
    }
}

MutaliskUnit::~MutaliskUnit()
{
}

void MutaliskUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("MutaliskUnit::micro non implémenté !");
#endif
}

void MutaliskUnit::check()
{
}

int MutaliskUnit::getAttackDuration()
{
    return 0;
}

std::set<BWAPI::UnitType> MutaliskUnit::getSetPrio()
{
    return MutaliskUnit::setPrio;
}