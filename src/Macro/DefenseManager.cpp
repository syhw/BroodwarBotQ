#include <PrecompiledHeader.h>
#include <DefenseManager.h>
#include <BorderManager.h>
DefenseManager::DefenseManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator)
{
  this->arbitrator = arbitrator;
}
void DefenseManager::setBorderManager(BorderManager* borderManager)
{
  this->borderManager=borderManager;
}
void DefenseManager::onOffer(std::set<BWAPI::Unit*> units)
{
  for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
  {
    if (defenders.find(*u) == defenders.end())
    {
      arbitrator->accept(this, *u);
      DefenseData temp;
      defenders.insert(std::make_pair(*u, temp));
    }
  }
}

void DefenseManager::onRevoke(BWAPI::Unit* unit, double bid)
{
  defenders.erase(unit);
}

void DefenseManager::onRemoveUnit(BWAPI::Unit* unit)
{
  defenders.erase(unit);
}

void DefenseManager::update()
{
  // Bid on all completed military units
  std::set<BWAPI::Unit*> myPlayerUnits=BWAPI::Broodwar->self()->getUnits();
  for (std::set<BWAPI::Unit*>::iterator u = myPlayerUnits.begin(); u != myPlayerUnits.end(); u++)
  {
    if ((*u)->isCompleted() && 
        !(*u)->getType().isWorker() && 
        !(*u)->getType().isBuilding() &&
        (*u)->getType() != BWAPI::UnitTypes::Zerg_Egg &&
        (*u)->getType() != BWAPI::UnitTypes::Zerg_Larva)
    {
      arbitrator->setBid(this, *u, 20);
    }
  }
  bool borderUpdated=false;
  if (myBorder!=borderManager->getMyBorder())
  {
    myBorder=borderManager->getMyBorder();
    myBorderVector.clear();
    for(std::set<BWTA::Chokepoint*>::iterator i=myBorder.begin();i!=myBorder.end();i++)
      myBorderVector.push_back(*i);
    borderUpdated=true;
  }
  //Order all units to choke
  int i=0;
  if (!myBorder.empty())
  {
    for (std::map<BWAPI::Unit*,DefenseData>::iterator u = defenders.begin(); u != defenders.end(); u++)
    {
      if ((*u).second.mode == DefenseData::Idle || borderUpdated)
      {
        BWAPI::Position chokePosition=myBorderVector[i]->getCenter();
        i++;
        if (i>=(int)myBorderVector.size())
          i=0;
        (*u).first->attack(chokePosition);
        (*u).second.mode = DefenseData::Moving;
      }
    }
  }
}

std::string DefenseManager::getName() const
{
  return "Defense Manager";
}

std::string DefenseManager::getShortName() const
{
  return "Def";
}