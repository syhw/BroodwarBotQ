#include <PrecompiledHeader.h>
#include "Macro/BasicTaskExecutor.h"
#include "Macro/MacroManager.h"
using namespace BWAPI;
BasicTaskExecutor* instance = NULL;
BasicTaskExecutor* BasicTaskExecutor::getInstance()
{
  if (instance==NULL)
    instance = new BasicTaskExecutor();
  return instance;
}
void BasicTaskExecutor::attached(TaskStream* ts)
{
  taskStreams[ts].isReady = false;
  taskStreams[ts].targetLevel = -1;
}
void BasicTaskExecutor::detached(TaskStream* ts)
{
  taskStreams.erase(ts);
}
void BasicTaskExecutor::newStatus(TaskStream* ts)
{
}
void BasicTaskExecutor::completedTask(TaskStream* ts, const Task &t)
{
}
void BasicTaskExecutor::update(TaskStream* ts)
{
  if (ts == NULL) return;
  if (ts->isLocationReady()==false) return;
  TaskTypes::Enum type = ts->getTask(0).getType();
  if (ts->getBuildUnit()!=NULL && ts->getBuildUnit()->exists())
  {
    Unit* unit = ts->getBuildUnit();
    Broodwar->drawTextMap(unit->getPosition().x(),unit->getPosition().y()+15,"r: %d, e: %d, c: %d",taskStreams[ts].isReady,ts->getTask(0).isExecuting(), ts->getTask(0).isCompleted());
  }
  else if (ts->isWorkerReady())
  {
    Unit* unit = ts->getWorker();
    Broodwar->drawTextMap(unit->getPosition().x(),unit->getPosition().y()+15,"r: %d, e: %d, c: %d",taskStreams[ts].isReady,ts->getTask(0).isExecuting(), ts->getTask(0).isCompleted());
  }
  if (ts->getStatus() != TaskStream::Executing_Task && ts->isWorkerReady())
  {
    Unit* worker = ts->getWorker();
    if (ts->getTask(0).getStartTime()>=0 &&
        ts->getTask(0).getType()==TaskTypes::Unit &&
        ts->getTask(0).getUnit().isBuilding() &&
        !ts->getTask(0).getWorkerType().isBuilding())
    {
      UnitType ut = ts->getTask(0).getUnit();
      Position targetPosition=Position(ts->getTask(0).getTilePosition());
      targetPosition.x()+=ut.tileWidth()*16;
      targetPosition.y()+=ut.tileHeight()*16;
      if (ts->getTask(0).getStartTime()<Broodwar->getFrameCount()+ts->getWorker()->getDistance(targetPosition)/ts->getTask(0).getWorkerType().topSpeed()*1.4+0.8*30)
      {
        if (worker->getDistance(targetPosition)>100 || (ut.isRefinery()==false && worker->getDistance(targetPosition)>32))
        {
          if (Broodwar->getFrameCount()>=ts->getWorker()->getLastCommandFrame()+5)
            worker->rightClick(targetPosition);
        }
      }
    }
    return;
  }
  if (ts->getTask(0).getType()==TaskTypes::Unit && ts->getTask(0).getUnit().isBuilding() &&
      ts->getBuildUnit()!=NULL && ts->getTask(0).getRace()==Races::Protoss)
  {
    if (ts->getTask(1).getType()!=TaskTypes::None)
    {
      TaskStream* newTS = ts->forkCurrentTask();
      TheMacroManager->taskStreams.push_back(newTS);
    }
    else
    {
      ts->setWorker(NULL);
    }
  }
  computeIsCompleted(ts);
  if (ts->getTask(0).isCompleted()) return;
  computeIsExecuting(ts);
  if (ts->getTask(0).isExecuting()) return;
  computeIsReady(ts);
  if (!taskStreams[ts].isReady)
    getReady(ts);
  else
    execute(ts);
}
void BasicTaskExecutor::computeIsExecuting(TaskStream* ts)
{
  ts->getTask(0).setExecuting(false);
  TaskTypes::Enum type = ts->getTask(0).getType();
  if (type == TaskTypes::Unit)
  {
    UnitType ut=ts->getTask(0).getUnit();
    if (ts->getBuildUnit()!=NULL && ts->getBuildUnit()->exists() && (ts->getBuildUnit()->getType() == ut || ts->getBuildUnit()->getBuildType() == ut) &&
      (ts->getTask(0).getRace()==Races::Protoss || ts->isWorkerReady()))
      ts->getTask(0).setExecuting(true);
    if (ts->isWorkerReady())
    {
      Unit* worker = ts->getWorker();
      if (Broodwar->getFrameCount()>worker->getLastCommandFrame()+5)
      {
        if (worker->getType()==UnitTypes::Terran_SCV && worker->isConstructing()==false)
          worker->rightClick(ts->getBuildUnit());
      }
    }
  }
  else if (type == TaskTypes::Tech)
  {
    if (ts->getWorker()!=NULL && ts->getWorker()->isResearching() && ts->getWorker()->getTech()==ts->getTask(0).getTech())
      ts->getTask(0).setExecuting(true);
  }
  else if (type == TaskTypes::Upgrade)
  {
    if (ts->getWorker()!=NULL && ts->getWorker()->isUpgrading() && ts->getWorker()->getUpgrade()==ts->getTask(0).getUpgrade())
      ts->getTask(0).setExecuting(true);
  }
  if (ts->getTask(0).isExecuting()) ts->getTask(0).setSpentResources(true);
}
void BasicTaskExecutor::computeIsCompleted(TaskStream* ts)
{
  if (ts->getTask(0).isCompleted()) return;
  TaskTypes::Enum type = ts->getTask(0).getType();
  if (type == TaskTypes::Unit)
  {
    if (ts->getBuildUnit()!=NULL &&
        ts->getBuildUnit()->exists() &&
        ts->getBuildUnit()->isCompleted() &&
        ts->getBuildUnit()->getType() == ts->getTask(0).getUnit())
    {
      ts->getTask(0).setCompleted(true);
    }
  }
  else if (type == TaskTypes::Tech)
  {
    if (Broodwar->self()->hasResearched(ts->getTask(0).getTech()))
      ts->getTask(0).setCompleted(true);
  }
  else if (type == TaskTypes::Upgrade)
  {
    if (taskStreams[ts].targetLevel>0 && Broodwar->self()->getUpgradeLevel(ts->getTask(0).getUpgrade()) >= taskStreams[ts].targetLevel)
      ts->getTask(0).setCompleted(true);
  }
}
void BasicTaskExecutor::computeIsReady(TaskStream* ts)
{
  if (taskStreams[ts].isReady) return;
  TaskTypes::Enum type = ts->getTask(0).getType();
  if (ts->isWorkerReady()==false)
    taskStreams[ts].isReady=false;
  else
  {
    Unit* worker = ts->getWorker();
    taskStreams[ts].isReady = !(worker->isResearching() || worker->isUpgrading() || worker->isTraining() || worker->isConstructing() || worker->isMorphing());
  }
}
void BasicTaskExecutor::getReady(TaskStream* ts)
{
  if (taskStreams[ts].isReady) return;
  if (ts->isWorkerReady()==false) return;
  Unit* worker = ts->getWorker();
  TaskTypes::Enum type = ts->getTask(0).getType();
  if (worker->isResearching() || worker->isUpgrading() || worker->isTraining() || worker->isConstructing() || worker->isMorphing())
    worker->stop();
}
void BasicTaskExecutor::execute(TaskStream* ts)
{
  if (ts->getTask(0).isCompleted()) return;
  if (ts->getTask(0).isExecuting()) return;
  if (taskStreams[ts].isReady==false) return;
  if (ts->isWorkerReady()==false) return;
  Unit* worker = ts->getWorker();
  TaskTypes::Enum type = ts->getTask(0).getType();
  if (Broodwar->getFrameCount()%5!=0) return;

  if (type == TaskTypes::Unit)
  {
    UnitType ut=ts->getTask(0).getUnit();
    if (ut.getRace()==Races::Zerg && ut.isBuilding()==ut.whatBuilds().first.isBuilding())
    {
      if (ut.whatBuilds().first == UnitTypes::Zerg_Larva && worker->getType().producesLarva())
      {
        Unit* larva = NULL;
        if (!worker->getLarva().empty())
          larva = *worker->getLarva().begin();
        if (larva!=NULL && larva->exists() && larva->morph(ut))
        {
          if (ts->getTask(0).hasSpentResources()==false)
          {
            ts->getTask(0).setSpentResources(true);
            TheMacroManager->spentResources+=ts->getTask(0).getResources();
          }
          TaskStream* newTS = ts->forkCurrentTask();
          TheMacroManager->taskStreams.push_back(newTS);
          newTS->setWorker(larva);
        }
      }
      else
      {
        if (worker->morph(ut))
        {
          if (ts->getTask(0).hasSpentResources()==false)
          {
            ts->getTask(0).setSpentResources(true);
            TheMacroManager->spentResources+=ts->getTask(0).getResources();
          }
        }
      }
    }
    else if (ut.isAddon())
    {
      if (worker->isLifted()==false)
      {
        if (worker->getTilePosition()!=ts->getTask(0).getTilePosition())
        {
          worker->lift();
        }
        else
        {
          if (worker->buildAddon(ut))
          {
            if (ts->getTask(0).hasSpentResources()==false)
            {
              ts->getTask(0).setSpentResources(true);
              TheMacroManager->spentResources+=ts->getTask(0).getResources();
            }
          }
        }
      }
      else
      {
        worker->land(ts->getTask(0).getTilePosition());
      }
    }
    else if (ut.isBuilding())
    {
      Position targetPosition=Position(ts->getTask(0).getTilePosition());
      targetPosition.x()+=ut.tileWidth()*16;
      targetPosition.y()+=ut.tileHeight()*16;
      if (worker->getDistance(targetPosition)>100)
        worker->rightClick(targetPosition);
      else
      {
        if (worker->build(ts->getTask(0).getTilePosition(),ut))
        {
          if (ts->getTask(0).hasSpentResources()==false)
          {
            ts->getTask(0).setSpentResources(true);
            TheMacroManager->spentResources+=ts->getTask(0).getResources();
          }
        }
      }
    }
    else
    {
      if (worker->train(ut))
      {
        if (ts->getTask(0).hasSpentResources()==false)
        {
          ts->getTask(0).setSpentResources(true);
          TheMacroManager->spentResources+=ts->getTask(0).getResources();
        }
      }
    }
  }
  else if (type == TaskTypes::Tech)
  {
    if (worker->research(ts->getTask(0).getTech()))
    {
      if (ts->getTask(0).hasSpentResources()==false)
      {
        ts->getTask(0).setSpentResources(true);
        TheMacroManager->spentResources+=ts->getTask(0).getResources();
      }
    }
  }
  else if (type == TaskTypes::Upgrade)
  {
    if (worker->upgrade(ts->getTask(0).getUpgrade()))
    {
      if (ts->getTask(0).hasSpentResources()==false)
      {
        ts->getTask(0).setSpentResources(true);
        taskStreams[ts].targetLevel = Broodwar->self()->getUpgradeLevel(ts->getTask(0).getUpgrade())+1;
        TheMacroManager->spentResources+=ts->getTask(0).getResources();
      }
    }
  }
}