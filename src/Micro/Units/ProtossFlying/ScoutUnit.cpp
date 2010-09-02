#include "ScoutUnit.h"

std::set<BWAPI::UnitType> ScoutUnit::setUnitsPrio;

ScoutUnit::ScoutUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setUnitsPrio.empty())
    {
        setUnitsPrio.insert(BWAPI::UnitTypes::Protoss_Corsair);
        setUnitsPrio.insert(BWAPI::UnitTypes::Protoss_Scout);
        setUnitsPrio.insert(BWAPI::UnitTypes::Protoss_Carrier);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Wraith);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Valkyrie);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Science_Vessel);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Battlecruiser);
        setUnitsPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
        setUnitsPrio.insert(BWAPI::UnitTypes::Zerg_Devourer);
        setUnitsPrio.insert(BWAPI::UnitTypes::Zerg_Scourge);
    }
}

ScoutUnit::~ScoutUnit()
{
}

void ScoutUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ScoutUnit::micro non implémenté !");
#endif
}

void ScoutUnit::check()
{
}

bool ScoutUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ScoutUnit::canHit non implémenté !");
#endif
    return false;
}

int ScoutUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ScoutUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

std::set<BWAPI::UnitType> ScoutUnit::getUnitsPrio()
{
    return ScoutUnit::setUnitsPrio;
}