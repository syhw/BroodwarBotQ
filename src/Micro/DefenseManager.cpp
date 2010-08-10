#include <DefenseManager.h>
#include <BorderManager.h>
#include "BaseManager.h"
#include "Base.h"
#include "DefendChokeGoal.h"
#include <map>
#include "WarManager.h"


using namespace BWTA;
DefenseManager::DefenseManager()
{
  this->arbitrator = NULL;
}
void DefenseManager::setDependencies(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arb, BorderManager* borderManager)
{
  this->arbitrator= arb;
  this->borderManager=borderManager;
}

void DefenseManager::checkDefenses(){
	//Check on all bases that if we need to defend it, then it is defended
	std::set<Base*> myBases =  BaseManager::Instance().getAllBases();
	for(std::set<Base*>::iterator it = myBases.begin(); it != myBases.end(); ++it){
		bool toDef=true;
		
		
		if((*it)->chokeToDef != NULL ) {
			//Should the chokepoint of this base be defended ?

			//Check if the next region contains a base of ours or not
			BWTA::Region* nextRegion;
			std::pair<Region*,Region*> regions = (*it)->chokeToDef->getRegions();
			if(regions.first == (*it)->getBaseLocation()->getRegion()){
				nextRegion = regions.second;
			} else {
				nextRegion = regions.first;
			}
			
			//Search nextRegion in our bases
			for(std::set<Base*>::iterator search = myBases.begin(); search != myBases.end(); ++search){
				if( (*search)->getBaseLocation()->getRegion() == nextRegion ){
					//The chokepoint does not need to be defended because the next Region is a base of ours
					toDef=false;
					break;
				}
			}
		}

		//If the field chokeToDef is NULL we consider we have to defend the base in the middle
		checkGroundDefense((*it),toDef);


	}

}

void DefenseManager::checkGroundDefense(Base * b, bool toDef){

	//Find the unitsGroup that must defend this base against ground attack
	assert(this->groundDefenders.count(b) == 1);
	UnitsGroup * ug = (*this->groundDefenders.find(b)).second;
	

	//Check if they must defend at middle or at chokepoint
	if(b->chokeToDef != NULL){
		//If they must defend the chokepoint check if their DefendChokeGoal is accomplished (enough units)
		if(toDef){
			if(ug->getLastGoal() != NULL){
				if( ug->getLastGoal()->getStatus() == GS_NOT_ENOUGH_UNITS){
					//We need more units
					
					//Set bid on 1 unit until we have enough and record the unitsgroup that needed it
					

					std::set<BWAPI::Unit *> allUnits = BWAPI::Broodwar->getAllUnits();
					for(std::set<BWAPI::Unit * >::iterator selection = allUnits.begin() ; selection != allUnits.end() ; ++selection){
						if((*selection)->getType() == BWAPI::UnitTypes::Protoss_Zealot || (*selection)->getType() == BWAPI::UnitTypes::Protoss_Dragoon ){
							this->requesters.push_back(ug);
							this->arbitrator->setBid(this, (*selection), 40);
							break;
						}
					}

				} else {	
				//Nothing to do, the defense at this chokepoint is correct
				
				}
			}
		} else {
			//We do not need to defend
			if(ug->getNbUnits() > 0){
				//We must give back those units to the WarManager
				for(std::vector<pBayesianUnit>::iterator units = (*ug->getUnits()).begin(); units != (*ug->getUnits()).end(); ++ units){
					this->arbitrator->setBid(this,(*units)->unit, 0);
					ug->giveUpControl((*units)->unit);
				}
			}
		}
		
	} else {
		//If they must defend in the middle of the base 


		//TODO
	}
}



void DefenseManager::onOffer(std::set<BWAPI::Unit*> units)
{
	for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++){
		
		if(this->requesters.size() != 0 ){
			this->requesters.front()->takeControl(*u);
			this->requesters.pop_front();
			this->arbitrator->accept(this, *u);

		} else {
			this->arbitrator->decline(this, *u, 0);
		}
	}
}

void DefenseManager::onRevoke(BWAPI::Unit* unit, double bid)
{
 // defenders.erase(unit);
}

void DefenseManager::onRemoveUnit(BWAPI::Unit* unit)
{
//  defenders.erase(unit);
}

void DefenseManager::update()
{
	this->checkDefenses();

}

void DefenseManager::addBase(Base * b){
	UnitsGroup * ug = new UnitsGroup();
	assert(this->groundDefenders.count(b) == 0);//We must not have already the groundDefenders for this base
	this->groundDefenders.insert(std::make_pair(b,ug));
	ug->addGoal(pGoal(new DefendChokeGoal(ug,b->chokeToDef)));
	WarManager::Instance().unitsgroups.push_back(ug);

}

std::string DefenseManager::getName() const
{
  return "Defense Manager";
}

std::string DefenseManager::getShortName() const
{
  return "Def";
}