#include "ScoutUnit.h"

ScoutUnit::ScoutUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

ScoutUnit::~ScoutUnit()
{
}

void ScoutUnit::micro()
{
    BWAPI::Broodwar->printf("ScoutUnit::micro non implémenté !");
}

const std::list<BWAPI::UnitType> ScoutUnit::getListePrio() const
{
    return listPriorite;
}

bool ScoutUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("ScoutUnit::canHit non implémenté !");
    return false;
}