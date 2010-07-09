#include "Subgoal.h"	
using namespace BWAPI;

Subgoal::Subgoal(SubgoalLogic l) :
logic(l)
{
}

Subgoal::Subgoal(const Subgoal& s) :
logic(s.getLogic())
{
}

Subgoal::~Subgoal(){
}

SubgoalLogic Subgoal::getLogic() const {
	return this->logic;
}
