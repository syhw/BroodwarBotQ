#include "ArchonUnit.h"

BWAPI::UnitType ArchonUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

ArchonUnit::ArchonUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
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

BWAPI::UnitType* ArchonUnit::getListPriorite()
{
    return ArchonUnit::listPriorite;
}