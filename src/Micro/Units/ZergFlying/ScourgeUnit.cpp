#include "ScourgeUnit.h"

std::set<BWAPI::UnitType> ScourgeUnit::setPrio;

ScourgeUnit::ScourgeUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_Corsair);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
        setPrio.insert(BWAPI::UnitTypes::Terran_Valkyrie);
    }
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

std::set<BWAPI::UnitType> ScourgeUnit::getSetPrio()
{
    return ScourgeUnit::setPrio;
}