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
    BWAPI::Broodwar->printf("CorsairUnit::micro non implémenté !");
}

bool CorsairUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("CorsairUnit::canHit non implémenté !");
    return false;
}

int CorsairUnit::getTimeToAttack()
{
    BWAPI::Broodwar->printf("CorsairUnit::getTimeToAttack non implémenté !");
    return 0;
}

BWAPI::UnitType* CorsairUnit::getListPriorite()
{
    return CorsairUnit::listPriorite;
}