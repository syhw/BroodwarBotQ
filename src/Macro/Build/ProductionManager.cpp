#include <ProductionManager.h>

ProductionManager::ProductionManager()
{
	arbitrator = &Arbitrator::Arbitrator<BWAPI::Unit*,double>::Instance();
	placer = &BuildingPlacer::Instance();
  for(std::set<BWAPI::UnitType>::iterator i=BWAPI::UnitTypes::allUnitTypes().begin();i!=BWAPI::UnitTypes::allUnitTypes().end();i++)
  {
    plannedCount[*i]=0;
    startedCount[*i]=0;
  }
}

ProductionManager::~ProductionManager()
{
}

void ProductionManager::onOffer(std::set<BWAPI::Unit*> units)
{
  for(std::set<BWAPI::Unit*>::iterator i=units.begin();i!=units.end();i++)
  {
    std::map<BWAPI::UnitType,std::list<BWAPI::UnitType> >::iterator q=productionQueues.find((*i)->getType());
    bool used=false;
    if (q!=productionQueues.end() && !q->second.empty())
    {
      for(std::list<BWAPI::UnitType>::iterator t=q->second.begin();t!=q->second.end();t++)
      {
        if (BWAPI::Broodwar->canMake(*i,*t))
        {
          producingUnits[*i].type=*t;
          producingUnits[*i].unit=NULL;
          producingUnits[*i].lastAttemptFrame=-100;
          producingUnits[*i].started=false;
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

void ProductionManager::onRevoke(BWAPI::Unit* unit, double bid)
{
  onUnitDestroy(unit);
}

void ProductionManager::update()
{
  std::set<BWAPI::Unit*> myPlayerUnits=BWAPI::Broodwar->self()->getUnits();
  for(std::set<BWAPI::Unit*>::iterator u = myPlayerUnits.begin(); u != myPlayerUnits.end(); u++)
  {
    std::map<BWAPI::UnitType,std::list<BWAPI::UnitType> >::iterator p=productionQueues.find((*u)->getType());
    if (p!=productionQueues.end() && !p->second.empty() && (*u)->isCompleted() && producingUnits.find(*u)==producingUnits.end())
      arbitrator->setBid(this, *u, 50);
  }

  std::map<BWAPI::Unit*,Unit>::iterator i_next;
  for(std::map<BWAPI::Unit*,Unit>::iterator i=producingUnits.begin();i!=producingUnits.end();i=i_next)
  {
    i_next=i;
    i_next++;
    if (i->first->isLifted())
    {
      if (i->first->isIdle())
        i->first->land(placer->getBuildLocationNear(i->first->getTilePosition()+BWAPI::TilePosition(0,1),i->first->getType()));
    }
    else
    {
      if (i->first->isTraining() && i->first->getBuildUnit()!=NULL)
        i->second.unit=i->first->getBuildUnit();

      if (i->second.unit==NULL)
      {
        if (BWAPI::Broodwar->getFrameCount()>i->second.lastAttemptFrame+BWAPI::Broodwar->getLatency()*2)
          if (BWAPI::Broodwar->canMake(i->first,i->second.type) && i->first->getTrainingQueue().size() < 3)
          {
            i->first->train(i->second.type);
            i->second.lastAttemptFrame=BWAPI::Broodwar->getFrameCount();
          }
      }
      else
      {
        if (!i->second.started)
        {
          startedCount[i->second.type]++;
          i->second.started=true;
        }

        if (i->second.unit->isCompleted())
        {
          if (i->second.unit->getType()==i->second.type)
          {
            arbitrator->removeBid(this, i->first);
            startedCount[i->second.type]--;
            plannedCount[i->second.type]--;
            producingUnits.erase(i);
          }
          else
            i->second.unit=NULL;
        }
        else
        {
          if (i->second.unit->exists() && i->second.unit->getType()!=i->second.type)
            i->first->cancelTrain();

          if (!i->first->isTraining())
            i->second.unit=NULL;
        }
      }
    }
  }
  std::map<BWAPI::UnitType, int> startedCount;

}

std::string ProductionManager::getName() const
{
  return "Production Manager";
}

void ProductionManager::onUnitDestroy(BWAPI::Unit* unit)
{
  if (producingUnits.find(unit)!=producingUnits.end())
  {
    if (productionQueues.find(unit->getType())!=productionQueues.end())
      productionQueues[unit->getType()].push_front(producingUnits[unit].type);
    producingUnits.erase(unit);
  }
}

bool ProductionManager::train(BWAPI::UnitType type)
{
  if (!type.whatBuilds().first.canProduce() || type.isBuilding())
    return false;
  if (type.getRace()==BWAPI::Races::Zerg)
      if (type.whatBuilds().first != BWAPI::UnitTypes::Zerg_Infested_Command_Center)
      return false;
  productionQueues[type.whatBuilds().first].push_back(type);
  plannedCount[type]++;
  return true;
}

int ProductionManager::getPlannedCount(BWAPI::UnitType type) const
{
  std::map<BWAPI::UnitType,int>::const_iterator i=plannedCount.find(type);
  if (i!=plannedCount.end())
    return i->second;
  return 0;
}

int ProductionManager::getStartedCount(BWAPI::UnitType type) const
{
  std::map<BWAPI::UnitType,int>::const_iterator i=startedCount.find(type);
  if (i!=startedCount.end())
    return i->second;
  return 0;
}