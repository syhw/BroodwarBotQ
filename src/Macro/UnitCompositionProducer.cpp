#include <PrecompiledHeader.h>
#include <UnitCompositionProducer.h>
#include <BasicTaskExecutor.h>
#include <SpiralBuildingPlacer.h>
#include <UnitPump.h>
#include <TerminateIfWorkerLost.h>
#include <TerminateIfEmpty.h>
#include <BasicWorkerFinder.h>
#include <MacroManager.h>
#include <MacroManager/UnitReadyTimeCalculator.h>
using namespace BWAPI;
using namespace std;
std::set<UnitType> requiresAddon;
std::map<UnitType,UnitType> getRequiredAddon;
UnitCompositionProducer::UnitCompositionProducer(BWAPI::UnitType workerType)
{
  this->workerType=workerType;
  if (requiresAddon.empty())
  {
    for each(UnitType t in UnitTypes::allUnitTypes())
    {
      for each(std::pair<UnitType, int> r in t.requiredUnits())
      {
        if (r.first.isAddon() && r.first.whatBuilds().first==t.whatBuilds().first)
        {
          requiresAddon.insert(t);
          getRequiredAddon[t]=r.first;
        }
      }
    }
  }
}
void UnitCompositionProducer::attached(TaskStream* ts)
{
  streams.insert(ts);
}
void UnitCompositionProducer::detached(TaskStream* ts)
{
  streams.erase(ts);
}
void UnitCompositionProducer::update()
{
  std::set<Unit*> units=Broodwar->self()->getUnits();
  for each(Unit* u in units)
  {
    if (u->getType()==workerType && u->exists())
    {
      if (TheMacroManager->getTaskStreams(u).empty())
      {
        TaskStream* ts = new TaskStream();
        TheMacroManager->taskStreams.push_back(ts);
        ts->setWorker(u);
        ts->attach(BasicTaskExecutor::getInstance(),false);
        ts->attach(this,false);
        ts->attach(new TerminateIfWorkerLost(),true);
      }
    }
  }

  actualUnitCounts.clear();
  for each(std::pair<UnitType, double> t in unitCompositionWeights)
  {
    actualUnitCounts[t.first]=TheMacroManager->uctl.getFinalCount(t.first);
  }
  for each(TaskStream* ts in streams)
  {
    for(int i=0;i<4;i++)
    {
      if (ts->getTask(i)==NULL || ts->getTask(i).getType() == TaskTypes::None)
      {
        UnitType t=getNextUnitType(ts->getWorker());
        if (t!=UnitTypes::None)
        {
          ts->setTask(i,Task(t,ts->getWorker()->getTilePosition()));
          actualUnitCounts[t]++;
        }
        break;
      }
    }
  }
}
UnitType UnitCompositionProducer::getNextUnitType(Unit* worker)
{
  /*

  int time = UnitReadyTimeCalculator::getReadyTime(worker,true);
  if (time == -1) return UnitTypes::None;

  int t2   = ts->getFinishTime();
  if (t2 == -1) return UnitTypes::None;

  time = max(time,t2);
  */

  double aTc = 0;
  double cTc = 0;
  for each(std::pair<UnitType, double> t in unitCompositionWeights)
  {
    aTc+=actualUnitCounts[t.first]*t.second;
    cTc+=t.second*t.second;
  }
  double r= aTc/cTc;
  for each(std::pair<UnitType, double> t in unitCompositionWeights)
  {
    desiredUnitCounts[t.first]=t.second*r;
  }
  int earliestFrame = -1;
  std::set<UnitType> choseFrom;
  for each(std::pair<UnitType, double> t in unitCompositionWeights)
  {
    UnitType typ = t.first;
    UnitReadyTimeStatus::Enum r;
    int t = UnitReadyTimeCalculator::getFirstFreeTime(worker,Task(typ),r,true,true);
    if (t==-1 && requiresAddon.find(typ)!=requiresAddon.end() && worker->getAddon()==NULL)
    {
      int addonFinishTime = -1;
      for each(TaskStream* ts in TheMacroManager->getTaskStreams(worker))
        if (addonFinishTime==-1)
          addonFinishTime = ts->getFinishTime(typ);
      if (addonFinishTime==-1)
      {
        typ = getRequiredAddon[typ];
        t = UnitReadyTimeCalculator::getFirstFreeTime(worker,Task(typ),r,true,true);
      }
    }
    if (earliestFrame == -1 || (t<earliestFrame && t!=-1))
    {
      choseFrom.clear();
      earliestFrame = t;
    }
    if (earliestFrame == t)
      choseFrom.insert(typ);
  }
  double maxDiff = -10000;
  UnitType nextType = UnitTypes::None;
  for each(UnitType typ in choseFrom)
  {
    double d = desiredUnitCounts[typ]-actualUnitCounts[typ];
    if (d>maxDiff)
    {
      maxDiff  = d;
      nextType = typ;
    }
  }
  return nextType;
}
void UnitCompositionProducer::setUnitWeight(BWAPI::UnitType t, double weight)
{
  unitCompositionWeights[t]=weight;
}