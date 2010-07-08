#include "ReaverUnit.h"

BWAPI::UnitType ReaverUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

ReaverUnit::ReaverUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

ReaverUnit::~ReaverUnit()
{
}

void ReaverUnit::micro()
{
    BWAPI::Broodwar->printf("ReaverUnit::micro non implémenté !");
}

bool ReaverUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("ReaverUnit::canHit non implémenté !");
    return false;
}

int ReaverUnit::getTimeToAttack()
{
    BWAPI::Broodwar->printf("ReaverUnit::getTimeToAttack non implémenté !");
    return 0;
}

BWAPI::UnitType* ReaverUnit::getListPriorite()
{
    return ReaverUnit::listPriorite;
}