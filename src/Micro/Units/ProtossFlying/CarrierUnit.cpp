#include "CarrierUnit.h"

BWAPI::UnitType CarrierUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

CarrierUnit::CarrierUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

CarrierUnit::~CarrierUnit()
{
}

void CarrierUnit::micro()
{
    BWAPI::Broodwar->printf("CarrierUnit::micro non implémenté !");
}

bool CarrierUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("CarrierUnit::canHit non implémenté !");
    return false;
}

int CarrierUnit::getTimeToAttack()
{
    BWAPI::Broodwar->printf("CarrierUnit::getTimeToAttack non implémenté !");
    return 0;
}

BWAPI::UnitType* CarrierUnit::getListPriorite()
{
    return CarrierUnit::listPriorite;
}