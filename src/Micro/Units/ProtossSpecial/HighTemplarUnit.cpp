#include "HighTemplarUnit.h"

BWAPI::UnitType HighTemplarUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

HighTemplarUnit::HighTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug):SpecialUnit(u, ug)
{
}

HighTemplarUnit::~HighTemplarUnit()
{
}

void HighTemplarUnit::micro()
{
    BWAPI::Broodwar->printf("HighTemplarUnit::micro non implémenté !");
}

bool HighTemplarUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("HighTemplarUnit::canHit non implémenté !");
    return false;
}

int HighTemplarUnit::getTimeToAttack()
{
    BWAPI::Broodwar->printf("HighTemplarUnit::getTimeToAttack non implémenté !");
    return 0;
}

BWAPI::UnitType* HighTemplarUnit::getListPriorite()
{
    return HighTemplarUnit::listPriorite;
}