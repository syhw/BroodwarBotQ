#include "ObserverUnit.h"

BWAPI::UnitType ObserverUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

ObserverUnit::ObserverUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

ObserverUnit::~ObserverUnit()
{
}

void ObserverUnit::micro()
{
    BWAPI::Broodwar->printf("ObserverUnit::micro non implémenté !");
}

bool ObserverUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("ObserverUnit::canHit non implémenté !");
    return false;
}

int ObserverUnit::getTimeToAttack()
{
    return 0;
}

BWAPI::UnitType* ObserverUnit::getListPriorite()
{
    return ObserverUnit::listPriorite;
}