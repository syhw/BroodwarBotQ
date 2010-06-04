#include <UpgradeManager.h>

UpgradeManager::UpgradeManager()
{
	arbitrator = & Arbitrator::Arbitrator<BWAPI::Unit*,double>::Instance();
  this->placer = NULL;
  for(std::set<BWAPI::UpgradeType>::iterator i=BWAPI::UpgradeTypes::allUpgradeTypes().begin();i!=BWAPI::UpgradeTypes::allUpgradeTypes().end();i++)
  {
    plannedLevel[*i]=0;
    startedLevel[*i]=0;
  }
}
void UpgradeManager::setBuildingPlacer(BuildingPlacer* placer)
{
  this->placer = placer;
}
void UpgradeManager::onOffer(std::set<BWAPI::Unit*> units)
{
  for(std::set<BWAPI::Unit*>::iterator i=units.begin();i!=units.end();i++)
  {
    std::map<BWAPI::UnitType,std::list<Upgrade> >::iterator q=upgradeQueues.find((*i)->getType());
    bool used=false;
    if (q!=upgradeQueues.end() && !q->second.empty())
    {
      for(std::list<Upgrade>::iterator t=q->second.begin();t!=q->second.end();t++)
      {
        if (BWAPI::Broodwar->canUpgrade(*i,t->type) && (*i)->isIdle())
        {
          upgradingUnits.insert(std::make_pair(*i,*t));
          q->second.erase(t);
          arbitrator->accept(this,*i);
          arbitrator->setBid(this,*i,100.0);
          used=true;
          break;
        }
      }
    }
    if (!used)
    {
      arbitrator->decline(this,*i,0);
      arbitrator->removeBid(this,*i);
    }
  }
}

void UpgradeManager::onRevoke(BWAPI::Unit* unit, double bid)
{
  this->onUnitDestroy(unit);
}

void UpgradeManager::update()
{
  std::set<BWAPI::Unit*> myPlayerUnits=BWAPI::Broodwar->self()->getUnits();
  for(std::set<BWAPI::Unit*>::iterator u = myPlayerUnits.begin(); u != myPlayerUnits.end(); u++)
  {
    std::map<BWAPI::UnitType,std::list<Upgrade> >::iterator r=upgradeQueues.find((*u)->getType());
    if (r!=upgradeQueues.end() && !r->second.empty())
      arbitrator->setBid(this, *u, 50);
  }
  std::map<BWAPI::Unit*,Upgrade>::iterator i_next;
  for(std::map<BWAPI::Unit*,Upgrade>::iterator i=upgradingUnits.begin();i!=upgradingUnits.end();i=i_next)
  {
    i_next=i;
    i_next++;
    if (i->first->isUpgrading())
    {
      if (i->first->getUpgrade()!=i->second.type)
      {
        i->first->cancelUpgrade();
      }
      else
      {
        if (startedLevel[i->second.type]<i->second.level)
          startedLevel[i->second.type]=i->second.level;
      }
    }
    else
    {
      if (BWAPI::Broodwar->self()->getUpgradeLevel(i->second.type)>=i->second.level)
      {
        upgradingUnits.erase(i);
        arbitrator->removeBid(this, i->first);
      }
      else
      {
        if (i->first->isLifted())
        {
          if (i->first->isIdle())
            i->first->land(placer->getBuildLocationNear(i->first->getTilePosition()+BWAPI::TilePosition(0,1),i->first->getType()));
        }
        else
        {
          if (BWAPI::Broodwar->canUpgrade(i->first,i->second.type))
            i->first->upgrade(i->second.type);
        }
      }
    }
  }
}

std::string UpgradeManager::getName() const
{
  return "Upgrade Manager";
}

void UpgradeManager::onUnitDestroy(BWAPI::Unit* unit)
{
  std::map<BWAPI::Unit*,Upgrade>::iterator r=upgradingUnits.find(unit);
  if (r!=upgradingUnits.end())
  {
    Upgrade type=r->second;
    if (BWAPI::Broodwar->self()->getUpgradeLevel(type.type)<type.level)
      upgradeQueues[type.type.whatUpgrades()].push_front(type);
    upgradingUnits.erase(r);
  }
}

bool UpgradeManager::upgrade(BWAPI::UpgradeType type)
{
  int level=BWAPI::Broodwar->self()->getUpgradeLevel(type)+1;
  if (level>type.maxRepeats()) return false;
  Upgrade newUpgrade;
  newUpgrade.type=type;
  newUpgrade.level=level;
  upgradeQueues[type.whatUpgrades()].push_back(newUpgrade);
  plannedLevel[type]=level;
  return true;
}

int UpgradeManager::getPlannedLevel(BWAPI::UpgradeType type) const
{
  std::map<BWAPI::UpgradeType, int>::const_iterator i=plannedLevel.find(type);
  if (i!=plannedLevel.end())
    return i->second;
  return 0;
}

int UpgradeManager::getStartedLevel(BWAPI::UpgradeType type) const
{
  std::map<BWAPI::UpgradeType, int>::const_iterator i=startedLevel.find(type);
  if (i!=startedLevel.end())
    return i->second;
  return 0;
}

int UpgradeManager::getCompletedLevel(BWAPI::UpgradeType type) const
{
  return BWAPI::Broodwar->self()->getUpgradeLevel(type);
}
