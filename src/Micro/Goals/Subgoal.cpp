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

void Subgoal::setUnitsGroup( UnitsGroup * ug){
unitsGroup = ug;
}

double Subgoal::distanceToRealize(BWAPI::Position){
	return this->distanceToRealize();
}