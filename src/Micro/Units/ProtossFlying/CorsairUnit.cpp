#include "CorsairUnit.h"

BWAPI::UnitType CorsairUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

CorsairUnit::CorsairUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

CorsairUnit::~CorsairUnit()
{
}

void CorsairUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("CorsairUnit::micro non implémenté !");
#endif
}

bool CorsairUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("CorsairUnit::canHit non implémenté !");
#endif
    return false;
}

int CorsairUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("CorsairUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

BWAPI::UnitType* CorsairUnit::getListPriorite()
{
    return CorsairUnit::listPriorite;
}