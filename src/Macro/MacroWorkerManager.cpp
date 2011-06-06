#include <PrecompiledHeader.h>
#include "Macro/MacroWorkerManager.h"
#include "Macro/MacroBaseManager.h"
#include "Macro/RectangleArray.h"
#include "Macro/MacroManager.h"
#include "Macro/UnitGroupManager.h"
#include "Utils/Util.h"
using namespace BWAPI;
using namespace std;
using namespace Util;

MacroWorkerManager* TheMacroWorkerManager = NULL;

MacroWorkerManager* MacroWorkerManager::create()
{
  if (TheMacroWorkerManager) return TheMacroWorkerManager;
  return new MacroWorkerManager();
}
void MacroWorkerManager::destroy()
{
  if (TheMacroWorkerManager)
    delete TheMacroWorkerManager;
}


MacroWorkerManager::MacroWorkerManager()
{
  TheMacroWorkerManager   = this;
  this->lastSCVBalance    = 0;
  this->WorkersPerGas     = 3;
  this->mineralRate       = 0;
  this->gasRate           = 0;
  this->autoBuild         = false;
  this->autoBuildPriority = 80;
}

MacroWorkerManager::~MacroWorkerManager()
{
  TheMacroWorkerManager = NULL;
}

void MacroWorkerManager::onOffer(set<Unit*> units)
{
  for(set<Unit*>::iterator u = units.begin(); u != units.end(); u++)
  {
    if ((*u)->getType().isWorker() && !this->mineralOrder.empty())
    {
      TheArbitrator->accept(this, *u);
      WorkerData temp;
      this->desiredWorkerCount[this->mineralOrder[this->mineralOrderIndex].first]++;
      this->currentWorkers[this->mineralOrder[this->mineralOrderIndex].first].insert(*u);
      temp.newResource = this->mineralOrder[this->mineralOrderIndex].first;
      this->mineralOrderIndex = (this->mineralOrderIndex+1) % mineralOrder.size();
      workers.insert(make_pair(*u,temp));
    }
    else
      TheArbitrator->decline(this, *u, 0);
  }
}
void MacroWorkerManager::onRevoke(Unit* unit, double bid)
{
  this->onRemoveUnit(unit);
}

void MacroWorkerManager::updateWorkerAssignments()
{
  //determine current worker assignments
  //also workers that are mining from resources that dont belong to any of our bases will be set to free

  for(map<Unit*,WorkerData >::iterator w = this->workers.begin(); w != this->workers.end(); w++)
  {
    if (w->second.newResource != NULL)
    {
      if (resourceBase.find(w->second.newResource) == resourceBase.end())
        w->second.newResource = NULL;
      else
        currentWorkers[w->second.newResource].insert(w->first);
    }
  }

  // get free workers
  set<Unit*> freeWorkers;
  for(map<Unit*,WorkerData>::iterator w = this->workers.begin(); w != this->workers.end(); w++)
  {
    if (w->second.newResource == NULL)
      freeWorkers.insert(w->first);
    else
    {
      // free workers that are too far away from their resources
      if (w->first->getDistance(w->second.newResource)>32*10)
      {
        freeWorkers.insert(w->first);
        //erase worker from resource's current workers set
        currentWorkers[w->second.newResource].erase(w->first);
      }
    }
  }

  // free workers from resources with too many workers
  for(map<Unit*,int>::iterator i = desiredWorkerCount.begin(); i != desiredWorkerCount.end(); i++)
    if (i->second < (int)currentWorkers[i->first].size())
    {
      // desired worker count is less than the current worker count for this resource, so lets remove some workers.
      int amountToRemove = currentWorkers[i->first].size() - i->second;
      for(int j = 0; j < amountToRemove; j++)
      {
        Unit* worker = *currentWorkers[i->first].begin();
        freeWorkers.insert(worker);
        workers[worker].newResource = NULL;
        currentWorkers[i->first].erase(worker);
      }
    }

  vector< Unit* > workerUnit;
  vector< Unit* > taskUnit;
  map<int,int> assignment;

  for(set<Unit*>::iterator i=freeWorkers.begin();i!=freeWorkers.end();i++)
    workerUnit.push_back(*i);

  // assign workers to resources that need more workers
  for(map<Unit*,int>::iterator i = desiredWorkerCount.begin(); i != desiredWorkerCount.end(); i++)
    if (i->second>(int)currentWorkers[i->first].size())
      for(int j=(int)currentWorkers[i->first].size();j<i->second;j++)
        taskUnit.push_back(i->first);

  //construct cost matrix
  //currently just uses euclidean distance, but walking distance would be more accurate
  RectangleArray<double> cost(workerUnit.size(),taskUnit.size());
  for(int w=0;w<(int)workerUnit.size();w++)
    for(int t=0;t<(int)taskUnit.size();t++)
      cost[w][t]=workerUnit[w]->getDistance(taskUnit[t]);

  //calculate assignment for workers and tasks (resources)
  assignment=computeAssignments(cost);

  //use assignment
  for(map<int,int>::iterator a=assignment.begin();a!=assignment.end();a++)
  {
    Unit* worker=workerUnit[a->first];
    Unit* resource=taskUnit[a->second];
    workers[worker].newResource = resource;
    currentWorkers[resource].insert(worker);
  }
}

bool mineralCompare (const pair<Unit*, int> i, const pair<Unit*, int> j) { return (i.second>j.second); }

void MacroWorkerManager::rebalanceWorkers()
{
  mineralOrder.clear();
  desiredWorkerCount.clear();
  currentWorkers.clear();
  resourceBase.clear();
  int remainingWorkers = this->workers.size();
  optimalWorkerCount = 0;
  
  // iterate over all the resources of each active base
  for(set<MacroBase*>::iterator b = this->basesCache.begin(); b != this->basesCache.end(); b++)
  {
    set<Unit*> baseMinerals = (*b)->getMinerals();
    vector< std::pair<Unit*,int> > baseMineralOrder;
    for(set<Unit*>::iterator m = baseMinerals.begin(); m != baseMinerals.end(); m++)
    {
      resourceBase[*m] = *b;
      desiredWorkerCount[*m] = 0;
      baseMineralOrder.push_back(std::make_pair(*m,(*m)->getResources() - 2*(int)(*m)->getPosition().getDistance((*b)->getBaseLocation()->getPosition())));
      optimalWorkerCount+=2;
    }
    sort(baseMineralOrder.begin(), baseMineralOrder.end(), mineralCompare);
    for(int i=0;i<(int)baseMineralOrder.size();i++)
    {
      Unit* mineral=baseMineralOrder[i].first;
      mineralOrder.push_back(make_pair(mineral, mineral->getResources() - 2*(int)mineral->getPosition().getDistance((*b)->getBaseLocation()->getPosition())-3000*i));
    }
    set<Unit*> baseGeysers = (*b)->getGeysers();
    for(set<Unit*>::iterator g = baseGeysers.begin(); g != baseGeysers.end(); g++)
    {
      optimalWorkerCount+=3;
      resourceBase[*g] = *b;
      desiredWorkerCount[*g]=0;

      if(remainingWorkers < 4)//always save some workers for minerals
        continue;

      if ((*g)->getType().isRefinery() && (*g)->getPlayer()==Broodwar->self() && (*g)->isCompleted())
      {
        for(int w=0;w<this->WorkersPerGas && remainingWorkers>0;w++)
        {
          desiredWorkerCount[*g]++;
          remainingWorkers--;
        }
      }
    }
  }

  //if no resources exist, return
  if (!mineralOrder.empty())
  {

    //order minerals by score (based on distance and resource amount)
    sort(mineralOrder.begin(), mineralOrder.end(), mineralCompare);

    //calculate optimal worker count for each mineral patch
    mineralOrderIndex = 0;
    while(remainingWorkers > 0)
    {
      desiredWorkerCount[mineralOrder[mineralOrderIndex].first]++;
      remainingWorkers--;
      mineralOrderIndex = (mineralOrderIndex + 1) % mineralOrder.size();
    }
  }

  //update the worker assignments so the actual workers per resource is the same as the desired workers per resource
  updateWorkerAssignments();
  if (this->autoBuild)
  {
    BWAPI::UnitType workerType=BWAPI::Broodwar->self()->getRace().getWorker();
    /*
    if (this->buildOrderManager->getPlannedCount(workerType)<optimalWorkerCount)
    {
      this->buildOrderManager->build(optimalWorkerCount,workerType,this->autoBuildPriority);
    }
    */
  }
}

void MacroWorkerManager::update()
{
  //bid a constant value of 10 on all completed workers
  set<Unit*> w = SelectAll()(isCompleted)(isWorker);
  for each(Unit* u in w)
    TheArbitrator->setBid(this, u, 10);

  //rebalance workers when necessary
  set<MacroBase*> bases = TheMacroBaseManager->getActiveBases();
  if (Broodwar->getFrameCount() > lastSCVBalance + 5*24 || bases != this->basesCache || lastSCVBalance == 0)
  {
    this->basesCache = bases;
    lastSCVBalance   = Broodwar->getFrameCount();
    this->rebalanceWorkers();
  }
  
  //order workers to gather from their assigned resources
  this->mineralRate=0;
  this->gasRate=0;
  for(map<Unit*,WorkerData>::iterator u = workers.begin(); u != workers.end(); u++)
  {
    Unit* i = u->first;
    if (u->second.resource!=NULL)
    {
      if (u->second.resource->getType()==UnitTypes::Resource_Mineral_Field)
        mineralRate+=8/180.0;
      else
        gasRate+=8/180.0;
    }
    
    //switch current resource to newResource when appropiate
    if (u->second.resource == NULL || (i->getTarget() != NULL && !i->getTarget()->getType().isResourceDepot()))
      u->second.resource = u->second.newResource;

    Unit* resource = u->second.resource;
    if (i->getOrder() == Orders::MoveToMinerals || 
        i->getOrder() == Orders::WaitForMinerals || 
        i->getOrder() == Orders::MoveToGas || 
        i->getOrder() == Orders::WaitForGas || 
        i->getOrder() == Orders::PlayerGuard)
      if ((i->getTarget()==NULL || !i->getTarget()->exists() || !i->getTarget()->getType().isResourceDepot()) && i->getTarget() != resource)
        i->rightClick(resource);
    if (i->isCarryingGas() || i->isCarryingMinerals())
    {
      if (i->getOrder() == Orders::ReturnGas || i->getOrder() == Orders::ReturnMinerals ||  (i->getOrder() == Orders::PlayerGuard && BWAPI::Broodwar->getFrameCount()>u->second.lastFrameSpam+BWAPI::Broodwar->getLatency()*2))
      {
        u->second.lastFrameSpam=BWAPI::Broodwar->getFrameCount();
        MacroBase* b=TheMacroBaseManager->getBase(BWTA::getNearestBaseLocation(i->getPosition()));
        if (b!=NULL)
        {
          Unit* center = b->getResourceDepot();
          if (i->getTarget()==NULL || !i->getTarget()->exists() || i->getTarget()!=center || (center->isCompleted() &&  i->getOrder() == Orders::PlayerGuard))
            i->rightClick(center);
        }
      }
    }
  }
}
string MacroWorkerManager::getName() const
{
  return "Worker Manager";
}

string MacroWorkerManager::getShortName() const
{
  return "Work";
}

void MacroWorkerManager::onRemoveUnit(Unit* unit)
{
  if (unit->getType().isWorker())
    workers.erase(unit);
  if (unit->getType().isResourceContainer())
  {
    map<Unit*,set<Unit*> >::iterator c=currentWorkers.find(unit);
    if (c==currentWorkers.end())
      return;
    for(set<Unit*>::iterator i=c->second.begin();i!=c->second.end();i++)
    {
      std::map<Unit*,WorkerData>::iterator j=workers.find(*i);
      if (j!=workers.end())
      {
        if (j->second.resource==unit)
          j->second.resource=NULL;
        if (j->second.newResource==unit)
          j->second.newResource=NULL;
      }
    }
  }
}

void MacroWorkerManager::setWorkersPerGas(int count)
{
  this->WorkersPerGas=count;
}
double MacroWorkerManager::getMineralRate() const
{
  return this->mineralRate;
}
double MacroWorkerManager::getGasRate() const
{
  return this->gasRate;
}
int MacroWorkerManager::getOptimalWorkerCount() const
{
  return this->optimalWorkerCount;
}
void MacroWorkerManager::enableAutoBuild()
{
  this->autoBuild=true;
}
void MacroWorkerManager::disableAutoBuild()
{
  this->autoBuild=false;
}
void MacroWorkerManager::setAutoBuildPriority(int priority)
{
  this->autoBuildPriority = priority;
}
