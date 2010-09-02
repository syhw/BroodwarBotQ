#include "ScourgeUnit.h"

BWAPI::UnitType ScourgeUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Zerg_Mutalisk,
                                                                BWAPI::UnitTypes::Zerg_Scourge,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

ScourgeUnit::ScourgeUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

ScourgeUnit::~ScourgeUnit()
{
}

void ScourgeUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ScourgeUnit::micro non implémenté !");
#endif
}

void ScourgeUnit::check()
{
}

bool ScourgeUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ScourgeUnit::canHit non implémenté !");
#endif
    return false;
}

int ScourgeUnit::getTimeToAttack()
{
    return 0;
}

BWAPI::UnitType* ScourgeUnit::getListPriorite()
{
    return ScourgeUnit::listPriorite;
}