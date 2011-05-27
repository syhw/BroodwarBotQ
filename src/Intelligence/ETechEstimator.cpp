#include <PrecompiledHeader.h>
#include "ETechEstimator.h"	

using namespace BWAPI;

Building::Building(UnitType t, double p)
: type(t)
, probToHave(p)
{
}

ETechEstimator::ETechEstimator()
{
}

void ETechEstimator::onUnitDestroy(Unit* u)
{
}

void ETechEstimator::onUnitShow(Unit* u)
{
}

void ETechEstimator::onUnitHide(Unit* u)
{
}