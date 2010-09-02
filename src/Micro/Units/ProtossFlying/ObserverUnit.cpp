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
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ObserverUnit::micro non implémenté !");
#endif
}

void ObserverUnit::check()
{
}

bool ObserverUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ObserverUnit::canHit non implémenté !");
#endif
    return false;
}

int ObserverUnit::getTimeToAttack()
{
    return 0;
}

std::set<BWAPI::UnitType> ObserverUnit::getUnitsPrio()
{
    return std::set<BWAPI::UnitType>();
}