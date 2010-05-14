#include <MorphManager.h>

MorphManager::MorphManager()
{
	arbitrator = & Arbitrator::Arbitrator<BWAPI::Unit*,double>::Instance();
  for(std::set<BWAPI::UnitType>::iterator i=BWAPI::UnitTypes::allUnitTypes().begin();i!=BWAPI::UnitTypes::allUnitTypes().end();i++)
  {
    plannedCount[*i]=0;
    startedCount[*i]=0;
  }
}

MorphManager::~MorphManager()
{
}

void MorphManager::onOffer(std::set<BWAPI::Unit*> units)
{
  for(std::set<BWAPI::Unit*>::iterator i=units.begin();i!=units.end();i++)
  {
    std::map<BWAPI::UnitType,std::list<BWAPI::UnitType> >::iterator q=morphQueues.find((*i)->getType());
    bool used=false;
    if (q!=morphQueues.end() && !q->second.empty())
    {
      for(std::list<BWAPI::UnitType>::iterator t=q->second.begin();t!=q->second.end();t++)
      {
        if (BWAPI::Broodwar->canMake(*i,*t))
        {
          Unit newUnit;
          newUnit.type=*t;
          newUnit.started=false;
          morphingUnits.insert(std::make_pair(*i,newUnit));
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

void MorphManager::onRevoke(BWAPI::Unit* unit, double bid)
{
  std::map<BWAPI::Unit*,Unit>::iterator m=morphingUnits.find(unit);
  if (m!=morphingUnits.end())
  {
    BWAPI::UnitType type=m->second.type;
    if (m->first->getType()!=type || m->first->isMorphing())
      morphQueues[*type.whatBuilds().first].push_front(type);
    morphingUnits.erase(m);
  }
}

void MorphManager::update()
{
  std::set<BWAPI::Unit*> myPlayerUnits=BWAPI::Broodwar->self()->getUnits();
  for(std::set<BWAPI::Unit*>::iterator u = myPlayerUnits.begin(); u != myPlayerUnits.end(); u++)
  {
    std::map<BWAPI::UnitType,std::list<BWAPI::UnitType> >::iterator q=morphQueues.find((*u)->getType());
    if (q!=morphQueues.end() && !q->second.empty() && (*u)->isCompleted() && morphingUnits.find(*u)==morphingUnits.end())
      arbitrator->setBid(this, *u, 50);
  }
  std::map<BWAPI::Unit*,Unit>::iterator i_next;
  for(std::map<BWAPI::Unit*,Unit>::iterator i=morphingUnits.begin();i!=morphingUnits.end();i=i_next)
  {
    i_next=i;
    i_next++;
    if (i->first->isCompleted())
    {
      if (i->first->getType()==i->second.type)
      {
        morphingUnits.erase(i);
        arbitrator->removeBid(this, i->first);
        plannedCount[i->second.type]--;
        startedCount[i->second.type]--;
      }
      else
        if (BWAPI::Broodwar->canMake(NULL,i->second.type))
          i->first->morph(i->second.type);
    }
    if (i->first->isMorphing())
    {
      if (!i->second.started)
      {
        startedCount[i->second.type]++;
        i->second.started=true;
      }
    }
  }
}

std::string MorphManager::getName() const
{
  return "Morph Manager";
}

void MorphManager::onUnitDestroy(BWAPI::Unit* unit)
{
  std::map<BWAPI::Unit*,Unit>::iterator m=morphingUnits.find(unit);
  if (m!=morphingUnits.end())
  {
    BWAPI::UnitType type=m->second.type;
    morphQueues[*type.whatBuilds().first].push_front(type);
    morphingUnits.erase(m);
  }
}

bool MorphManager::morph(BWAPI::UnitType type)
{
  if (type.isBuilding()!=type.whatBuilds().first->isBuilding())
    return false;
  morphQueues[*type.whatBuilds().first].push_back(type);
  plannedCount[type]++;
  return true;
}

int MorphManager::getPlannedCount(BWAPI::UnitType type) const
{
  std::map<BWAPI::UnitType,int>::const_iterator i=plannedCount.find(type);
  if (i!=plannedCount.end())
    return i->second;
  return 0;
}

int MorphManager::getStartedCount(BWAPI::UnitType type) const
{
  std::map<BWAPI::UnitType,int>::const_iterator i=startedCount.find(type);
  if (i!=startedCount.end())
    return i->second;
  return 0;
}