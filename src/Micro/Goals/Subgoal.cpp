#include <PrecompiledHeader.h>
#include "Subgoal.h"	
using namespace BWAPI;

Subgoal::Subgoal(SubgoalLogic l)
: logic(l)
{
}

Subgoal::Subgoal(const Subgoal& sg) 
: logic(sg.logic)
{
}

Subgoal::~Subgoal()
{
}

SubgoalLogic Subgoal::getLogic() const
{
	return this->logic;
}

void Subgoal::setUnitsGroup( UnitsGroup * ug)
{
    unitsGroup = ug;
}