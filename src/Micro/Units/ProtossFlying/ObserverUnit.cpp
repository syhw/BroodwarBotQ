#include "ObserverUnit.h"

ObserverUnit::ObserverUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

ObserverUnit::~ObserverUnit()
{
}

void ObserverUnit::micro()
{

}

const std::list<BWAPI::UnitType> ObserverUnit::getListePrio() const
{
    return listPriorite;
}