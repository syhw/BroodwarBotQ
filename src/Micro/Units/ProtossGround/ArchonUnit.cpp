#include "ArchonUnit.h"

std::set<BWAPI::UnitType> ArchonUnit::setUnitsPrio;

ArchonUnit::ArchonUnit(BWAPI::Unit* u, UnitsGroup* ug)
: GroundUnit(u, ug)
{
    if (setUnitsPrio.empty())
    {
        setUnitsPrio.insert(BWAPI::UnitTypes::Protoss_Zealot);
        setUnitsPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Marine);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Firebat);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Medic);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Ghost);
        setUnitsPrio.insert(BWAPI::UnitTypes::Zerg_Zergling);
        setUnitsPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
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

std::set<BWAPI::UnitType> ArchonUnit::getUnitsPrio()
{
    return ArchonUnit::setUnitsPrio;
}