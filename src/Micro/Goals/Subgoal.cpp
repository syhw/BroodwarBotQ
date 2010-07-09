#include "Subgoal.h"	
using namespace BWAPI;

Subgoal::Subgoal(SubgoalLogic l) :
logic(l)
{
}

Subgoal::~Subgoal(){
}

SubgoalLogic Subgoal::getLogic() const {
	return this->logic;
}
