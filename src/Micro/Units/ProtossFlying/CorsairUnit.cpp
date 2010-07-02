#include "CorsairUnit.h"

CorsairUnit::CorsairUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

CorsairUnit::~CorsairUnit()
{
}

void CorsairUnit::micro()
{

}

const std::list<BWAPI::UnitType> CorsairUnit::getListePrio() const
{
    return listPriorite;
}