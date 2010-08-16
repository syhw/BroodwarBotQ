#include <BaseManager.h>
using namespace BWTA;
Base::Base(BWTA::BaseLocation* location)
{
  this->baseLocation     = location;
  this->resourceDepot    = NULL;
  this->refinery         = NULL;
  this->active           = false;
  this->activeGas        = false;
  this->beingConstructed = false;
  this->chokeToDef = NULL;
  computeChoke();
}

void Base::computeChoke(){
 //Compute chokeToDef :
	std::set<Chokepoint*> chokes = this->baseLocation->getRegion()->getChokepoints();
	if(chokes.size() > 1){
		//remove all bases in contact that have only 1 chokepoint (with this base)
		for(std::set<Chokepoint*>::iterator it = chokes.begin(); it != chokes.end(); ++it){
			std::pair<Region*,Region*> p = (*it)->getRegions();
			if((p.first==this->baseLocation->getRegion() && p.second->getChokepoints().size()==1) || 
				(p.second==this->baseLocation->getRegion() && p.first->getChokepoints().size()==1)){
				chokes.erase(it);
				continue;
			}
		}
	}
	this->chokeToDef = NULL;

	if(chokes.size() == 1){
		this->chokeToDef = (*chokes.begin());
		//Compute the size
		std::pair<BWAPI::Position,BWAPI::Position> sides = chokeToDef->getSides();
		double dist = sides.first.getDistance(sides.second);


		//Check if the base in contact is easier to defend
		std::pair<Region*,Region*> p = chokeToDef->getRegions();
		if( p.first == this->baseLocation->getRegion() && p.second->getChokepoints().size() == 2){
			//Get the good chokepoint (not the one with this base)
			BWTA::Chokepoint* chokeToTest;
			for(std::set<Chokepoint*>::const_iterator it = p.second->getChokepoints().begin(); it != p.second->getChokepoints().end(); ++it){
				if( (*it)!= chokeToDef){
					chokeToTest = (*it);
					break;
				}
			}
			//Compare the size 
			std::pair<BWAPI::Position,BWAPI::Position> sidesToTest = chokeToTest->getSides();
			if( 2 * sidesToTest.first.getDistance(sidesToTest.second) <= dist ){
				this->chokeToDef = chokeToTest;
			}
		} else if( p.second == this->baseLocation->getRegion() && p.first->getChokepoints().size() == 2){

			//Get the good chokepoint (not the one with this base)
			BWTA::Chokepoint* chokeToTest;
			for(std::set<Chokepoint*>::const_iterator it = p.first->getChokepoints().begin(); it != p.first->getChokepoints().end(); ++it){
				if( (*it)!= chokeToDef){
					chokeToTest = (*it);
					break;
				}
			}
			//Compare the size 
			std::pair<BWAPI::Position,BWAPI::Position> sidesToTest = chokeToTest->getSides();
			if( 2 * sidesToTest.first.getDistance(sidesToTest.second) <= dist ){
				this->chokeToDef = chokeToTest;
			}
		}
	}

}
BWTA::BaseLocation* Base::getBaseLocation() const
{
  return this->baseLocation;
}

BWAPI::Unit* Base::getResourceDepot() const
{
  return this->resourceDepot;
}

BWAPI::Unit* Base::getRefinery() const
{
  return this->refinery;
}

std::set<BWAPI::Unit*> Base::getMinerals() const
{
  return this->baseLocation->getMinerals();
}

std::set<BWAPI::Unit*> Base::getGeysers() const
{
  return this->baseLocation->getGeysers();
}

bool Base::isActive() const
{
  return this->active;
}

bool Base::isActiveGas() const
{
  return this->activeGas;
}

bool Base::isBeingConstructed() const
{
  return this->beingConstructed;
}

bool Base::hasGas() const
{
  bool gas = false;

  //if Baselocation has a vespene geyser
  if(!(this->getBaseLocation()->isMineralOnly()))
  {
    gas = true;
  }
  return gas;
}

void Base::setResourceDepot(BWAPI::Unit* unit)
{
  this->resourceDepot = unit;
}

void Base::setRefinery(BWAPI::Unit* unit)
{
  this->refinery = unit;
}

void Base::setActive(bool active)
{
  this->active = active;
}

void Base::setActiveGas(bool active)
{
  this->activeGas = active;
}
