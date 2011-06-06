#include <PrecompiledHeader.h>
#include "Macro/MacroBaseManager.h"
#include "Macro/MacroManager.h"
#include "Macro/BasicTaskExecutor.h"
#include "Macro/BFSBuildingPlacer.h"
#include "Macro/TerminateIfEmpty.h"
#include "Macro/BasicWorkerFinder.h"
#include "Macro/Task.h"
using namespace BWAPI;
std::set<Unit*> emptySet;
MacroBase* MacroBase::CreateBaseNow(BWTA::BaseLocation* b, bool getGas)
{
  MacroBase* mb = new MacroBase(b);
  Task s(Broodwar->self()->getRace().getCenter(),b->getTilePosition());
  mb->depot_ts = new TaskStream(s);
  TheMacroManager->taskStreams.push_front(mb->depot_ts);
  mb->depot_ts->attach(new BasicWorkerFinder(),true);
  mb->depot_ts->attach(BasicTaskExecutor::getInstance(),false);
  mb->depot_ts->attach(new TerminateIfEmpty(),true);
  mb->depot_ts->attach(BFSBuildingPlacer::getInstance(),false); 
  mb->depot_ts->attach(mb,false);
  BFSBuildingPlacer::getInstance()->setRelocatable(mb->depot_ts,false);
  return mb;
}
MacroBase* MacroBase::CreateBaseWhenPossible(BWTA::BaseLocation* b, bool getGas)
{
  MacroBase* mb = new MacroBase(b);
  Task s(Broodwar->self()->getRace().getCenter(),b->getTilePosition());
  mb->depot_ts = new TaskStream(s);
  TheMacroManager->taskStreams.push_back(mb->depot_ts);
  mb->depot_ts->attach(new BasicWorkerFinder(),true);
  mb->depot_ts->attach(BasicTaskExecutor::getInstance(),false);
  mb->depot_ts->attach(new TerminateIfEmpty(),true);
  mb->depot_ts->attach(BFSBuildingPlacer::getInstance(),false);
  mb->depot_ts->attach(mb,false);
  BFSBuildingPlacer::getInstance()->setRelocatable(mb->depot_ts,false);
  return mb;
}
MacroBase* MacroBase::CreateBaseAtFrame(BWTA::BaseLocation* b, int frame, bool getGas)
{
  MacroBase* mb = new MacroBase(b);
  Task s(Broodwar->self()->getRace().getCenter(),b->getTilePosition());
  s.setEarliestStartTime(frame);
  mb->depot_ts = new TaskStream(s);
  TheMacroManager->taskStreams.push_front(mb->depot_ts);
  mb->depot_ts->attach(new BasicWorkerFinder(),true);
  mb->depot_ts->attach(BasicTaskExecutor::getInstance(),false);
  mb->depot_ts->attach(new TerminateIfEmpty(),true);
  mb->depot_ts->attach(BFSBuildingPlacer::getInstance(),false);
  mb->depot_ts->attach(mb,false);
  BFSBuildingPlacer::getInstance()->setRelocatable(mb->depot_ts,false);
  return mb;
}
MacroBase* MacroBase::RecognizeExistingBase(BWTA::BaseLocation* b, BWAPI::Unit* depot)
{
  MacroBase* mb = new MacroBase(b);
  mb->resourceDepot = depot;
  return mb;
}
TaskStream* MacroBase::rebuildBase()
{
  if (depot_ts!=NULL) return depot_ts;
  return NULL;
}
TaskStream* MacroBase::rebuildRefinery()
{
  if (refinery_ts!=NULL) return refinery_ts;
  return NULL;
}
BWTA::BaseLocation* MacroBase::getBaseLocation() const
{
  return baseLocation;
}

BWAPI::Unit* MacroBase::getResourceDepot() const
{
  return resourceDepot;
}

BWAPI::Unit* MacroBase::getRefinery() const
{
  return refinery;
}

const std::set<BWAPI::Unit*>& MacroBase::getMinerals() const
{
  if (baseLocation==NULL)
    return emptySet;
  return baseLocation->getMinerals();
}

const std::set<BWAPI::Unit*>& MacroBase::getGeysers() const
{
  if (baseLocation==NULL)
    return emptySet;
  return baseLocation->getGeysers();
}
TaskStream* MacroBase::getTaskStream() const
{
  return depot_ts;
}
TaskStream* MacroBase::getRefineryTaskStream() const
{
  return refinery_ts;
}
void MacroBase::completedTask(TaskStream* ts, const Task &t)
{
  if (ts == depot_ts)
  {
    resourceDepot = ts->getBuildUnit();
    depot_ts = NULL;
  }
  else if (ts == refinery_ts)
  {
    refinery = ts->getBuildUnit();
    refinery_ts = NULL;
  }
}

void MacroBase::setPaused(bool paused)
{
  this->paused = paused;
}
bool MacroBase::isPaused() const
{
  return paused;
}
bool MacroBase::isReady() const
{
  return ready;
}
bool MacroBase::isActive() const
{
  return !paused && ready;
}

void MacroBase::update()
{
  if (resourceDepot == NULL)
  {
    if (depot_ts!=NULL)
      resourceDepot = depot_ts->getBuildUnit();
  }
  if (refinery = NULL)
  {
    if (refinery_ts!=NULL)
      refinery = refinery_ts->getBuildUnit();
  }
  ready = (resourceDepot && resourceDepot->exists() && (resourceDepot->isCompleted() || resourceDepot->getRemainingBuildTime()<300));
}
void MacroBase::onUnitDestroy(BWAPI::Unit* u)
{
  if (u==refinery)
    refinery = NULL;
  if (u==resourceDepot)
    resourceDepot = NULL;
}
MacroBase::MacroBase(BWTA::BaseLocation* b)
{
  baseLocation     = b;
  resourceDepot    = NULL;
  refinery         = NULL;
  ready            = false;
  depot_ts         = NULL;
  refinery_ts      = NULL;
  paused           = false;
}
