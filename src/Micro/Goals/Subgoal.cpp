#include <PrecompiledHeader.h>
#include "Subgoal.h"	

using namespace BWAPI;

Subgoal::Subgoal(SubgoalLogic l, UnitsGroup* ug)
: _logic(l)
, _unitsGroup(ug)
{
}

Subgoal::Subgoal(const Subgoal& sg) 
: _logic(sg._logic)
, _unitsGroup(sg._unitsGroup)
{
}

Subgoal::~Subgoal()
{
}

SubgoalLogic Subgoal::getLogic() const
{
	return _logic;
}

void Subgoal::setUnitsGroup(UnitsGroup * ug)
{
    _unitsGroup = ug;
}