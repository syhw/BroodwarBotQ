#include "CorsairUnit.h"

std::set<BWAPI::UnitType> CorsairUnit::setPrio;

CorsairUnit::CorsairUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_Corsair);
        setPrio.insert(BWAPI::UnitTypes::Terran_Wraith);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Scourge);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
    }
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

void CorsairUnit::check()
{
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

std::set<BWAPI::UnitType> CorsairUnit::getSetPrio()
{
    return CorsairUnit::setPrio;
}