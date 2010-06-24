#include "Subgoal.h"	
using namespace BWAPI;


Subgoal::Subgoal():validated(false){

}

Subgoal::~Subgoal(){
}

BWAPI::Position Subgoal::subgoalPosition() const {
	return this->pos;
}

SubgoalType Subgoal::subgoalType() const {
	return this->type;
}

SubgoalCondition Subgoal::subgoalCondition() const {
	return this->cond;
}

bool Subgoal::isValid() {

	if(this->cond==SC_ONCE){

		if(!validated)
			this->validated=check();

		return validated;

	}else if (this->cond==SC_ACTIVE) {
		return check();
		
	}
		
}

bool Subgoal::check(){
	if (type==ST_REACH) {
		//TODO
		return false;
	}else if (type==ST_REACH_BY_UG) {
		//TODO	
		return false;
	}else if (type==ST_VIEW){
		return Broodwar->isVisible(pos.x(),pos.y());
	}
	
}