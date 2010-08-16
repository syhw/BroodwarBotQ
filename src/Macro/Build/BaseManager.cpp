#include <BaseManager.h>
#include <BuildOrderManager.h>
#include <BorderManager.h>

BaseManager::BaseManager()
{
  this->builder = NULL;
  this->RefineryNeeded  = 1;
  this->refineryBuildPriority = 0;
  computeNatural();
}

void BaseManager::setDependencies(){
	this->builder = & BuildOrderManager::Instance();
	this->borderManager = & BorderManager::Instance();
	this->defenseManager = & DefenseManager::Instance();
}

void BaseManager::update()
{
  for(std::set<Base*>::iterator b = this->allBases.begin(); b != this->allBases.end(); b++)
  {
    if (!(*b)->isActive())
    {
      if ((*b)->getResourceDepot() == NULL)
      {
        BWAPI::TilePosition tile = (*b)->getBaseLocation()->getTilePosition();
        std::set<BWAPI::Unit*> units = BWAPI::Broodwar->unitsOnTile(tile.x(),tile.y());
        for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
          if ((*u)->getPlayer() == BWAPI::Broodwar->self() && (*u)->getType().isResourceDepot())
          {
            (*b)->setResourceDepot(*u);
            break;
          }
      }
      if ((*b)->getResourceDepot()!=NULL)
      {
        if ((*b)->getResourceDepot()->exists()==false)
          (*b)->setResourceDepot(NULL);
        else
        {
          if ((*b)->getResourceDepot()->isCompleted() || (*b)->getResourceDepot()->getRemainingBuildTime() < 250)
            (*b)->setActive(true);
        }
      }
    }

    //Set Refinerys
    if (!(*b)->isActiveGas() && (*b)->hasGas())
    {
      if ((*b)->getRefinery() == NULL)
      {
        std::set<BWAPI::Unit*> baseGeysers = (*b)->getBaseLocation()->getGeysers();

        BWAPI::TilePosition geyserLocation;

        //cycle through geysers & get tile location
        for(std::set<BWAPI::Unit*>::iterator bg = baseGeysers.begin(); bg != baseGeysers.end(); bg++)
        {
          geyserLocation = (*bg)->getTilePosition();
        }

        //check for refinery already on geyser
        std::set<BWAPI::Unit*> unitsOnGeyser = BWAPI::Broodwar->unitsOnTile(geyserLocation.x(),geyserLocation.y());
        
        for(std::set<BWAPI::Unit*>::iterator u = unitsOnGeyser.begin(); u != unitsOnGeyser.end(); u++)
        {
          if ((*u)->getPlayer() == BWAPI::Broodwar->self() && (*u)->getType().isRefinery())
          {
            (*b)->setRefinery(*u);
            break;
          }
        }
      }

      if ((*b)->getRefinery() != NULL)
      {
        if ((*b)->getRefinery()->exists()==false)
          (*b)->setResourceDepot(NULL);
        else
        {
          if ((*b)->getRefinery()->isCompleted() || (*b)->getRefinery()->getRemainingBuildTime() < 250)
            (*b)->setActiveGas(true);
        }
      }
    }
  }

  //check to see if any new base locations need to be added
  for(std::set<BWTA::BaseLocation*>::const_iterator bl = BWTA::getBaseLocations().begin(); bl != BWTA::getBaseLocations().end(); bl++)
  {
    if (location2base.find(*bl) == location2base.end())
    {
      BWAPI::TilePosition tile = (*bl)->getTilePosition();
      std::set<BWAPI::Unit*> units = BWAPI::Broodwar->unitsOnTile(tile.x(), tile.y());
      for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
        if ((*u)->getPlayer() == BWAPI::Broodwar->self() && (*u)->getType().isResourceDepot())
          addBase(*bl);
    }
  }

  if(BWAPI::Broodwar->getFrameCount() % 500 == 0 && BWAPI::Broodwar->getFrameCount() >= 3000)
    updateRefineries();

}


void BaseManager::updateRefineries()
{
    //if refinerys needed, build ONE refinery.
    if (this->isRefineryNeeded())
    {
      std::set<Base*> gasBases = this->getAllBasesWithGas();

      for(std::set<Base*>::iterator b = gasBases.begin(); b != gasBases.end(); b++)
      {
        BWTA::BaseLocation* location = (*b)->getBaseLocation();
        if (!this->hasRefinery(location))
        {
           this->builder->buildAdditional(1,BWAPI::Broodwar->self()->getRace().getRefinery(),refineryBuildPriority,(*b)->getBaseLocation()->getTilePosition());
           break;
        }
      }
    }


}

bool BaseManager::isRefineryNeeded()
{
  return (this->RefineryNeeded > this->builder->getPlannedCount(BWAPI::Broodwar->self()->getRace().getRefinery()));
}

void BaseManager::setRefineryBuildPriority(int priority)
{
  this->refineryBuildPriority = priority;
}

int BaseManager::getRefineryBuildPriority()
{
  return this->refineryBuildPriority;
}

void BaseManager::addBase(BWTA::BaseLocation* location)
{
  Base* newBase = new Base(location);
  allBases.insert(newBase);
  this->location2base[location] = newBase;
  this->borderManager->addMyBase(location);
  defenseManager->addBase(newBase);
}

void BaseManager::removeBase(BWTA::BaseLocation* location)
{
  std::map<BWTA::BaseLocation*,Base*>::iterator removebase;

  removebase = this->location2base.find(location);
  this->location2base.erase(removebase);
}
Base* BaseManager::getBase(BWTA::BaseLocation* location)
{
  std::map<BWTA::BaseLocation*,Base*>::iterator i=location2base.find(location);
  if (i==location2base.end())
    return NULL;
  return i->second;
}


void BaseManager::expand(int priority)
{
  BWTA::BaseLocation* location=NULL;
  double minDist=-1;
  BWTA::BaseLocation* home=BWTA::getStartLocation(BWAPI::Broodwar->self());
  for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
  {
    double dist=home->getGroundDistance(*i);
    if (dist>0 && getBase(*i)==NULL)
    {
      if (minDist == -1 || dist<minDist)
      {
        minDist=dist;
        location=*i;
      }
    }
  }
  expand(location,priority);
}
void BaseManager::expand(BWTA::BaseLocation* location, int priority)
{
  if (location==NULL)
    return;
  addBase(location);
  this->builder->buildAdditional(1,BWAPI::Broodwar->self()->getRace().getCenter(),priority,location->getTilePosition());

  if(!(location->isMineralOnly()))  
  {
    this->RefineryNeeded+=1;

    if (!(this->hasRefinery(location)))
      this->builder->buildAdditional(1,BWAPI::Broodwar->self()->getRace().getRefinery(),priority,location->getTilePosition());
  }
}


std::set<Base*> BaseManager::getActiveBases() const
{
  std::set<Base*> activeBases;
  for(std::set<Base*>::const_iterator b = this->allBases.begin(); b != this->allBases.end(); b++)
    if ((*b)->isActive())
      activeBases.insert(*b);
  return activeBases;
}
std::set<Base*> BaseManager::getAllBases() const
{
  std::set<Base*> allBases;
  for(std::set<Base*>::const_iterator b = this->allBases.begin(); b != this->allBases.end(); b++)
    allBases.insert(*b);
  return allBases;
}

std::set<Base*> BaseManager::getAllBasesWithGas()
{
  std::set<Base*> allBasesWithGas;

  for(std::set<Base*>::iterator b = this->allBases.begin(); b != this->allBases.end(); b++)
    if ((*b)->hasGas())
    {
      allBasesWithGas.insert(*b);
    }
  return allBasesWithGas;
}

std::string BaseManager::getName()
{
  return "Base Manager";
}
void BaseManager::onRemoveUnit(BWAPI::Unit* unit)
{
  for(std::set<Base*>::const_iterator b = this->allBases.begin(); b != this->allBases.end(); b++)
  {
    if((*b)->getResourceDepot() == unit)
    {
      if (unit->isCompleted())
      {
        this->borderManager->removeMyBase((*b)->getBaseLocation());
        
        BWTA::BaseLocation* blocation  = (*b)->getBaseLocation();
        removeBase(blocation);
        allBases.erase(b);
      }
      else
        (*b)->setResourceDepot(NULL);
      break;
    }
    else if((*b)->getRefinery() == unit)
    {
      (*b)->setRefinery(NULL);
      (*b)->setActiveGas(false);
      break;
    }
  }
}


bool BaseManager::hasRefinery(BWTA::BaseLocation* location)
{
  bool refinery = false;

  //if base has gas
  if(!(location->isMineralOnly()))
  {
    std::set<BWAPI::Unit*> basegeysers = location->getGeysers();

    BWAPI::TilePosition geyserlocation;

    //cycle through geysers & get tile location
    for(std::set<BWAPI::Unit*>::iterator bg = basegeysers.begin(); bg != basegeysers.end(); bg++)
    {
      geyserlocation = (*bg)->getInitialTilePosition();
    }
     
    //check for refinery already on geyser

    //get units on geyser
    std::set<BWAPI::Unit*> unitsOnGeyser = BWAPI::Broodwar->unitsOnTile(geyserlocation.x(),geyserlocation.y());
      
    //cycle through units on geyser
    for(std::set<BWAPI::Unit*>::iterator u = unitsOnGeyser.begin(); u != unitsOnGeyser.end(); u++)
    {
      //if unit is a refinery
      if ((*u)->getType().isRefinery())
      {
        refinery = true;
      }
    }
  }
  
  return refinery;
}

void BaseManager::computeNatural(){

	double minDist = 10000000000;
	double test;
	BWTA::BaseLocation * minBase;
	std::set<BWTA::BaseLocation * > allBaseLocations = BWTA::getBaseLocations();
	BWTA::BaseLocation * myBaseLocation = BWTA::getStartLocation(BWAPI::Broodwar->self());
	
	for(std::set<BWTA::BaseLocation *>::iterator it = allBaseLocations.begin(); it != allBaseLocations.end(); ++it){
		if( (*it) !=  myBaseLocation && !(*it)->isMineralOnly() ){
			//not our main
			test = (*it)->getPosition().getDistance(myBaseLocation->getPosition());
			if(test < minDist){
				minDist = test;
				minBase = (*it);
			}
		}
	}
	this->naturalExpand = minBase;
}
