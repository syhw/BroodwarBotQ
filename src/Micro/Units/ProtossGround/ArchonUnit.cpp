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
    BWAPI::Broodwar->printf("ArchonUnit::micro non implémenté !");
}

bool ArchonUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("ArchonUnit::canHit non implémenté !");
    return false;
}

int ArchonUnit::getTimeToAttack()
{
    BWAPI::Broodwar->printf("ArchonUnit::getTimeToAttack non implémenté !");
    return 0;
}

BWAPI::UnitType* ArchonUnit::getListPriorite()
{
    return ArchonUnit::listPriorite;
}