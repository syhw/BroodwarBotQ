#include <PrecompiledHeader.h>
#include "GroundUnit.h"

ProbTables GroundUnit::_sProbTables = ProbTables();

GroundUnit::GroundUnit(BWAPI::Unit* u, UnitsGroup* ug)
: BayesianUnit(u, ug, &_sProbTables)
{
}

GroundUnit::GroundUnit(BWAPI::Unit* u, UnitsGroup* ug,
					   const ProbTables* probTables)
: BayesianUnit(u, ug, probTables)
{
}

GroundUnit::~GroundUnit()
{
}