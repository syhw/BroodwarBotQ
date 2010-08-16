#include "DefendChokeGoal.h"
#include "SeeSubgoal.h"
#include "FormationSubgoal.h"
#include "Subgoal.h"
#include "Formations.h"


DefendChokeGoal::DefendChokeGoal(UnitsGroup * ug,BWTA::Chokepoint * choke) : 
Goal(ug)
{
this->choke = choke;
if(choke != NULL){
	double aa = this->choke->getSides().first.x() - this->choke->getSides().second.x();
	double bb = this->choke->getSides().first.y() - this->choke->getSides().second.y();  
	this->addSubgoal(pSubgoal(new FormationSubgoal(SL_AND,pFormation(new LineFormation(this->choke->getCenter(), Vec(bb/aa,1).normalize())))));
}
}



DefendChokeGoal::~DefendChokeGoal(){

}

void DefendChokeGoal::achieve(){
	this->checkAchievement();
	if(this->status != GS_ACHIEVED){
		//If they are not inPos make them move
		(*this->subgoals.begin())->tryToRealize();
	}
}

void DefendChokeGoal::checkAchievement(){
	//Check if we have the right amount of units. If not, set GS_NOT_ENOUGH_UNITS and return
	if(this->unitsGroup == NULL){
		//Strange...But must be avoided
	}else{
		if(this->enoughUnits() ||  this->unitsGroup->getNbUnits() < 4){
			this->status = GS_NOT_ENOUGH_UNITS;
		}else{
			BWAPI::Broodwar->printf("ok enough");
			this->status = GS_ACHIEVED;
		}
	}

	//Create appropriate goal of formation in the chokepoint and accomplish it



}
	
bool DefendChokeGoal::enoughUnits(){
	if(this->choke != NULL){
		double sizeChoke = this->choke->getSides().first.getDistance(choke->getSides().second);
		//Sum size of units
		double size= 0;
		for(std::vector<pBayesianUnit>::iterator it = this->unitsGroup->units.begin(); it != this->unitsGroup->units.end(); ++it){
			size += (*it)->_slarge;
		}
		if(size < sizeChoke + BWAPI::UnitTypes::Zerg_Zergling.dimensionLeft() + BWAPI::UnitTypes::Zerg_Zergling.dimensionRight()){
			return false;
		}
		return true;
	}else{
		return true;
	}
}
