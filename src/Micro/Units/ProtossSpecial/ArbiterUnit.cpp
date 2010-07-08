#include "ArbiterUnit.h"

BWAPI::UnitType ArbiterUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

ArbiterUnit::ArbiterUnit(BWAPI::Unit* u, UnitsGroup* ug):SpecialUnit(u, ug)
{
}

ArbiterUnit::~ArbiterUnit()
{
}

void ArbiterUnit::micro()
{
    BWAPI::Broodwar->printf("ArbiterUnit::micro non implémenté !");
}

bool ArbiterUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("ArbiterUnit::canHit non implémenté !");
    return false;
}

int ArbiterUnit::getTimeToAttack()
{
    BWAPI::Broodwar->printf("ArbiterUnit::getTimeToAttack non implémenté !");
    return 0;
}

BWAPI::UnitType* ArbiterUnit::getListPriorite()
{
    return ArbiterUnit::listPriorite;
}