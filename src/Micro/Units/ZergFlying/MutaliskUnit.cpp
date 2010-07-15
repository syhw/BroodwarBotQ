#include "MutaliskUnit.h"

BWAPI::UnitType MutaliskUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Zerg_Scourge,
                                                                BWAPI::UnitTypes::Zerg_Mutalisk,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

MutaliskUnit::MutaliskUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

MutaliskUnit::~MutaliskUnit()
{
}

void MutaliskUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("MutaliskUnit::micro non implémenté !");
#endif
}

bool MutaliskUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("MutaliskUnit::canHit non implémenté !");
#endif
    return false;
}

int MutaliskUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("MutaliskUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

BWAPI::UnitType* MutaliskUnit::getListPriorite()
{
    return MutaliskUnit::listPriorite;
}