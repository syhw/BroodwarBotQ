#include <PrecompiledHeader.h>
#include "FlyingUnit.h"

ProbTables FlyingUnit::_sProbTables = ProbTables();

FlyingUnit::FlyingUnit(BWAPI::Unit* u, UnitsGroup* ug)
: BayesianUnit(u, ug, &_sProbTables)
{
}

FlyingUnit::~FlyingUnit()
{
}