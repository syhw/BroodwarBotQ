#include <PrecompiledHeader.h>
#include "FlyingUnit.h"

ProbTables FlyingUnit::_sProbTables = ProbTables(-2); // -2 for flying

FlyingUnit::FlyingUnit(BWAPI::Unit* u, UnitsGroup* ug)
: BayesianUnit(u, ug, &_sProbTables)
{
}

FlyingUnit::~FlyingUnit()
{
}