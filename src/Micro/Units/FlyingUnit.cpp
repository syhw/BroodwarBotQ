#include <PrecompiledHeader.h>
#include "FlyingUnit.h"

ProbTables FlyingUnit::_sProbTables = ProbTables(-2); // -2 for flying

FlyingUnit::FlyingUnit(BWAPI::Unit* u)
: BayesianUnit(u, &_sProbTables)
{
}

FlyingUnit::~FlyingUnit()
{
}