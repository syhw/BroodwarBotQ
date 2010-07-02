#include "CarrierUnit.h"

CarrierUnit::CarrierUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

CarrierUnit::~CarrierUnit()
{
}

void CarrierUnit::micro()
{
    BWAPI::Broodwar->printf("CarrierUnit::micro non implémenté !");
}

const std::list<BWAPI::UnitType> CarrierUnit::getListePrio() const
{
    return listPriorite;
}

bool CarrierUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("CarrierUnit::canHit non implémenté !");
    return false;
}