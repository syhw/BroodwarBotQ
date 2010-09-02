#include "CorsairUnit.h"

std::set<BWAPI::UnitType> CorsairUnit::setUnitsPrio;

CorsairUnit::CorsairUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setUnitsPrio.empty())
    {
        setUnitsPrio.insert(BWAPI::UnitTypes::Protoss_Corsair);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Wraith);
        setUnitsPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
        setUnitsPrio.insert(BWAPI::UnitTypes::Zerg_Scourge);
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

std::set<BWAPI::UnitType> CorsairUnit::getUnitsPrio()
{
    return CorsairUnit::setUnitsPrio;
}