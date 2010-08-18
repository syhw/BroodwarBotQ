#include <SupplyManager.h>
#include <UnitGroupManager.h>
SupplyManager::SupplyManager()
{
  this->buildManager      = NULL;
  this->buildOrderManager = NULL;
  this->lastFrameCheck    = 0;
  this->seedPosition      = BWAPI::TilePositions::None;
}

void SupplyManager::setDependencies(){
	this->buildManager = & BuildManager::Instance();
	this->buildOrderManager = & BuildOrderManager::Instance();

}

void SupplyManager::update()
{
  if (BWAPI::Broodwar->getFrameCount()>lastFrameCheck+25)
  {
    lastFrameCheck=BWAPI::Broodwar->getFrameCount();
    int productionCapacity       = 0;
    lastFrameCheck               = BWAPI::Broodwar->getFrameCount();
    std::set<BWAPI::Unit*> units = BWAPI::Broodwar->self()->getUnits();
    int supplyBuildTime = BWAPI::Broodwar->self()->getRace().getSupplyProvider().buildTime();
    int time = BWAPI::Broodwar->getFrameCount() + supplyBuildTime*2;
    for(std::set<BuildOrderManager::MetaUnit*>::iterator i = this->buildOrderManager->MetaUnitPointers.begin(); i != this->buildOrderManager->MetaUnitPointers.end(); i++)
    {
      std::set<BWAPI::UnitType> m=this->buildOrderManager->unitsCanMake(*i,time);
      int max=0;
      for(std::set<BWAPI::UnitType>::iterator j=m.begin();j!=m.end();j++)
      {
        int s=j->supplyRequired();
        if (j->isTwoUnitsInOneEgg())
          s*=2;
        if (j->buildTime()<supplyBuildTime && (*i)->getType().getRace()!=BWAPI::Races::Zerg)
          s*=2;
        if (s > max)
          max=s;
      }
      productionCapacity += max;
    }
    if (getPlannedSupply() <= BWAPI::Broodwar->self()->supplyUsed() + productionCapacity)
    {
      this->buildOrderManager->buildAdditional(1,BWAPI::Broodwar->self()->getRace().getSupplyProvider(),1000,seedPosition);
    }
  }
}

std::string SupplyManager::getName() const
{
  return "Supply Manager";
}

int SupplyManager::getPlannedSupply() const
{
  int plannedSupply=0;
  //planned supply depends on the the amount of planned supply providers times the amount of supply they provide.
  plannedSupply+=buildOrderManager->getPlannedCount(BWAPI::UnitTypes::Terran_Supply_Depot)*BWAPI::UnitTypes::Terran_Supply_Depot.supplyProvided();
  plannedSupply+=buildOrderManager->getPlannedCount(BWAPI::UnitTypes::Protoss_Pylon)*BWAPI::UnitTypes::Protoss_Pylon.supplyProvided();
  plannedSupply+=buildOrderManager->getPlannedCount(BWAPI::UnitTypes::Zerg_Overlord)*BWAPI::UnitTypes::Zerg_Overlord.supplyProvided();

  plannedSupply+=SelectAll(BWAPI::UnitTypes::Terran_Command_Center).size()*BWAPI::UnitTypes::Terran_Command_Center.supplyProvided();
  plannedSupply+=SelectAll(BWAPI::UnitTypes::Protoss_Nexus).size()*BWAPI::UnitTypes::Protoss_Nexus.supplyProvided();
  plannedSupply+=SelectAll(BWAPI::UnitTypes::Zerg_Hatchery).size()*BWAPI::UnitTypes::Zerg_Hatchery.supplyProvided();
  plannedSupply+=SelectAll(BWAPI::UnitTypes::Zerg_Lair).size()*BWAPI::UnitTypes::Zerg_Lair.supplyProvided();
  plannedSupply+=SelectAll(BWAPI::UnitTypes::Zerg_Hive).size()*BWAPI::UnitTypes::Zerg_Hive.supplyProvided();
  return plannedSupply;
}
int SupplyManager::getSupplyTime(int supplyCount) const
{
  if (getPlannedSupply()<supplyCount)
    return -1; //not planning to make this much supply

  if (BWAPI::Broodwar->self()->supplyTotal()>=supplyCount)
    return BWAPI::Broodwar->getFrameCount(); //already have this much supply

  int supply=BWAPI::Broodwar->self()->supplyTotal();
  int time = BWAPI::Broodwar->getFrameCount();
  std::set<BWAPI::Unit*> units = SelectAll()(-isCompleted);
  std::map<int, int> supplyAdditions;
  for(std::set<BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
  {
    if ((*i)->getType().supplyProvided()>0)
    {
      supplyAdditions[time+(*i)->getRemainingBuildTime()]+=(*i)->getType().supplyProvided();
    }
  }
  for(std::map<int,int>::iterator i=supplyAdditions.begin();i!=supplyAdditions.end();i++)
  {
    time=i->second;
    supply+=i->first;
    if (supply>=supplyCount)
      return time;
  }
  return -1;
}

void SupplyManager::setSeedPosition(BWAPI::TilePosition p)
{
  this->seedPosition = p;
}