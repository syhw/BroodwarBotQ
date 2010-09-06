#include "DefendBaseGroundGoal.h"
#include "SeeSubgoal.h"
#include "FormationSubgoal.h"
#include "Subgoal.h"
#include "Formations.h"


DefendBaseGroundGoal::DefendBaseGroundGoal(UnitsGroup * ug, Base * b) : 
Goal(ug)
{
    this->choke = b->chokeToDef;
    if(choke != NULL){
        double deltaX = this->choke->getSides().first.x() - this->choke->getSides().second.x();
        double deltaY = this->choke->getSides().first.y() - this->choke->getSides().second.y();
        double tmp = 1.0;
        if (!deltaX)
        {
            deltaX = 1.0;
            tmp = 0.0;
        }
        this->addSubgoal(pSubgoal(new FormationSubgoal(SL_AND,pFormation(new LineFormation(this->choke->getCenter(), Vec(deltaY/deltaX, tmp).normalize())))));
    } else {
        addSubgoal(pSubgoal(new FormationSubgoal(SL_AND,pFormation(new SquareFormation(b->getBaseLocation()->getPosition(),Vec(0,1))))));

    }
}

DefendBaseGroundGoal::~DefendBaseGroundGoal()
{
}

void DefendBaseGroundGoal::achieve(){
	this->checkAchievement();
	if(this->status != GS_ACHIEVED){
		//If they are not inPos make them move
		(*this->subgoals.begin())->tryToRealize();
	}
}

void DefendBaseGroundGoal::checkAchievement(){
	//Check if we have the right amount of units. If not, set GS_NOT_ENOUGH_UNITS and return
	if(this->unitsGroup == NULL){
		//Strange...But must be avoided
	}else{
		if(!this->enoughUnits() && this->unitsGroup->size() < 3){
			this->status = GS_NOT_ENOUGH_UNITS;
		}else{
			this->status = GS_ACHIEVED;
		}
	}

	//Create appropriate goal of formation in the chokepoint and accomplish it

}
	
bool DefendBaseGroundGoal::enoughUnits(){
	//Return true if we have enough units to wall the chokepoint (if the chokepoint is not null)
	if(this->choke != NULL){
		double sizeChoke = this->choke->getSides().first.getDistance(choke->getSides().second);
		//Sum size of units
		double size= 0;
		for(std::vector<pBayesianUnit>::iterator it = this->unitsGroup->units.begin(); it != this->unitsGroup->units.end(); ++it){
			size += (*it)->_slarge;
		}
		if(size + BWAPI::UnitTypes::Zerg_Zergling.dimensionLeft() + BWAPI::UnitTypes::Zerg_Zergling.dimensionRight() < sizeChoke ){
			return false;
		}
		return true;
	}else{
		return false;
	}
}
