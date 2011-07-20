#include <PrecompiledHeader.h>
#include "Macro/SupplyManager.h"
#include <Macro/UnitGroupManager.h>
#include <Macro/Builder.h>

using namespace BWAPI;
using namespace std;

SupplyManager* TheSupplyManager = NULL;

SupplyManager* SupplyManager::create()
{
	if (TheSupplyManager)
		return TheSupplyManager;
	TheSupplyManager = new SupplyManager();
	return TheSupplyManager;
}

void SupplyManager::destroy()
{
	if (TheSupplyManager)
		delete TheSupplyManager;
}

SupplyManager::SupplyManager()
: lastFrameCheck   (0)
{
}

SupplyManager::~SupplyManager()
{
	TheSupplyManager = NULL;
}

void SupplyManager::update()
{
  if (BWAPI::Broodwar->getFrameCount() > lastFrameCheck+25)
  {
    lastFrameCheck = BWAPI::Broodwar->getFrameCount();
  }
}

/*
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
*/