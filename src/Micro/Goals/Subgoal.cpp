#include "Subgoal.h"	
using namespace BWAPI;


Subgoal::Subgoal() : validated(false){

}

Subgoal::Subgoal(SubgoalType t, SubgoalCondition c, BWAPI::Position pos) : validated(false),type(t),cond(c),pos(pos){

}

Subgoal::Subgoal(SubgoalType t, SubgoalCondition c, pFormation formation) :
validated(false),
type(t),
cond(c),
formation(formation)
{
	if(t==	ST_REACH_BY_UG || t == ST_REACH || t == ST_VIEW || t == ST_ATTACK){
		BWAPI::Broodwar->printf("ERROR, the subgoal created has a type that does not use formation");
		//TODO : create the position using the formation goal
	}

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

bool Subgoal::isRealized() {
//TOCHECK

	switch(this->cond){

		case SC_ONCE : {
			if(!validated)
				this->validated=check();
			return validated;
			break;
					  }
		case SC_ACTIVE : {

			return check();
			break;	
						 }

		default : {
			return check();
				  }
	}
}

bool Subgoal::check(){

	switch(this->type){

		case ST_REACH : {
			//TODO
			break;
						}

		case ST_REACH_BY_UG : {

			//TODO
			break;	
							  }

		case ST_VIEW : {

			return Broodwar->isVisible(pos.x()/32,pos.y()/32);
			break;	
					   }
		case ST_ATTACK :{

			//TODO
			break;
						}
		case ST_REACH_F : {
			
			//TODO
			break;
						  }
		case ST_ATTACK_F : {

			//TODO
			break;
						   }
							  
		default : {
			return true;
				  }
	}
	return true;
}