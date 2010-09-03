#include "ArchonUnit.h"

std::set<BWAPI::UnitType> ArchonUnit::setPrio;

ArchonUnit::ArchonUnit(BWAPI::Unit* u, UnitsGroup* ug)
: GroundUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Zergling);
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Zealot);
        setPrio.insert(BWAPI::UnitTypes::Terran_Firebat);
        setPrio.insert(BWAPI::UnitTypes::Terran_Marine);
        setPrio.insert(BWAPI::UnitTypes::Terran_Medic);
        setPrio.insert(BWAPI::UnitTypes::Terran_Ghost);
    }
}

ArchonUnit::~ArchonUnit()
{
}

void ArchonUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ArchonUnit::micro non implémenté !");
#endif
}

void ArchonUnit::check()
{
}

bool ArchonUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ArchonUnit::canHit non implémenté !");
#endif
    return false;
}

int ArchonUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ArchonUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

std::set<BWAPI::UnitType> ArchonUnit::getSetPrio()
{
    return ArchonUnit::setPrio;
}