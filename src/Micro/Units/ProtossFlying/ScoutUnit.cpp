#include "ScoutUnit.h"

std::set<BWAPI::UnitType> ScoutUnit::setPrio;

ScoutUnit::ScoutUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_Corsair);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Scout);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Carrier);
        setPrio.insert(BWAPI::UnitTypes::Terran_Wraith);
        setPrio.insert(BWAPI::UnitTypes::Terran_Valkyrie);
        setPrio.insert(BWAPI::UnitTypes::Terran_Science_Vessel);
        setPrio.insert(BWAPI::UnitTypes::Terran_Battlecruiser);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Scourge);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Devourer);
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

std::set<BWAPI::UnitType> ScoutUnit::getSetPrio()
{
    return ScoutUnit::setPrio;
}