#include "CarrierUnit.h"

CarrierUnit::CarrierUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

CarrierUnit::~CarrierUnit()
{
}

void CarrierUnit::micro()
{

}

const std::list<BWAPI::UnitType> CarrierUnit::getListePrio() const
{
    return listPriorite;
}