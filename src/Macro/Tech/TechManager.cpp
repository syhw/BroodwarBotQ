#include <TechManager.h>

TechManager::TechManager()
{
	this->arbitrator = & Arbitrator::Arbitrator<BWAPI::Unit*,double>::Instance();
  this->placer = NULL;
}

TechManager::~TechManager()
{
}

void TechManager::setBuildingPlacer(BuildingPlacer* placer)
{
  this->placer = placer;
}

void TechManager::onOffer(std::set<BWAPI::Unit*> units)
{
  for(std::set<BWAPI::Unit*>::iterator i=units.begin();i!=units.end();i++)
  {
    std::map<BWAPI::UnitType,std::list<BWAPI::TechType> >::iterator q=researchQueues.find((*i)->getType());
    bool used=false;
    if (q!=researchQueues.end() && !q->second.empty())
    {
      for(std::list<BWAPI::TechType>::iterator t=q->second.begin();t!=q->second.end();t++)
      {
        if (BWAPI::Broodwar->canResearch(*i,*t) && (*i)->isIdle())
        {
          researchingUnits.insert(std::make_pair(*i,*t));
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

void TechManager::onRevoke(BWAPI::Unit* unit, double bid)
{
  this->onUnitDestroy(unit);
}

void TechManager::update()
{
  std::set<BWAPI::Unit*> myPlayerUnits=BWAPI::Broodwar->self()->getUnits();
  for(std::set<BWAPI::Unit*>::iterator u = myPlayerUnits.begin(); u != myPlayerUnits.end(); u++)
  {
    std::map<BWAPI::UnitType,std::list<BWAPI::TechType> >::iterator r=researchQueues.find((*u)->getType());
    if (r!=researchQueues.end() && !r->second.empty())
      arbitrator->setBid(this, *u, 50);
  }
  std::map<BWAPI::Unit*,BWAPI::TechType>::iterator i_next;
  for(std::map<BWAPI::Unit*,BWAPI::TechType>::iterator i=researchingUnits.begin();i!=researchingUnits.end();i=i_next)
  {
    i_next=i;
    i_next++;
    if (i->first->isResearching())
    {
      if (i->first->getTech()!=i->second)
      {
        i->first->cancelResearch();
      }
    }
    else
    {
      if (BWAPI::Broodwar->self()->hasResearched(i->second))
      {
        researchingUnits.erase(i);
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
          if (BWAPI::Broodwar->canResearch(i->first,i->second))
            i->first->research(i->second);
        }
      }
    }
  }
}

std::string TechManager::getName() const
{
  return "Tech Manager";
}

void TechManager::onUnitDestroy(BWAPI::Unit* unit)
{
  std::map<BWAPI::Unit*,BWAPI::TechType>::iterator r=researchingUnits.find(unit);
  if (r!=researchingUnits.end())
  {
    BWAPI::TechType type=r->second;
    if (!BWAPI::Broodwar->self()->hasResearched(type))
      researchQueues[type.whatResearches()].push_front(type);
    researchingUnits.erase(r);
  }
}

bool TechManager::research(BWAPI::TechType type)
{
  researchQueues[type.whatResearches()].push_back(type);
  plannedTech.insert(type);
  return true;
}

bool TechManager::planned(BWAPI::TechType type) const
{
  std::set<BWAPI::TechType>::const_iterator i=plannedTech.find(type);
  return (i!=plannedTech.end());
}