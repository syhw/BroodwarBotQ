#include <PrecompiledHeader.h>
#include <MacroDependencyResolver.h>
#include <MacroManager.h>
#include <ResourceRates.h>
#include <BasicTaskExecutor.h>
#include <SpiralBuildingPlacer.h>
#include <BFSBuildingPlacer.h>
#include <UnitPump.h>
#include <TerminateIfWorkerLost.h>
#include <TerminateIfEmpty.h>
#include <BasicWorkerFinder.h>
#include <UnitCompositionProducer.h>
using namespace BWAPI;
using namespace std;
MacroDependencyResolver* TheMacroDependencyResolver = NULL;
MacroDependencyResolver* MacroDependencyResolver::create()
{
  if (TheMacroDependencyResolver) return TheMacroDependencyResolver;
  return new MacroDependencyResolver();
}
void MacroDependencyResolver::destroy()
{
  if (TheMacroDependencyResolver)
    delete TheMacroDependencyResolver;
}
MacroDependencyResolver::MacroDependencyResolver()
{
  TheMacroDependencyResolver=this;
}
MacroDependencyResolver::~MacroDependencyResolver()
{
  TheMacroDependencyResolver = NULL;
}
void MacroDependencyResolver::update()
{
  //Warning: Doesn't handle terran add-ons correctly yet
  if (Broodwar->getFrameCount()>lastFrameCheck+25)
  {
    lastFrameCheck=Broodwar->getFrameCount();
    std::map<UnitType,int> finalCounts;
    for each(UnitType t in UnitTypes::allUnitTypes())
    {
      finalCounts[t]=TheMacroManager->uctl.getFinalCount(t);
    }
    for(list<TaskStream*>::iterator tsi = TheMacroManager->taskStreams.begin(); tsi!= TheMacroManager->taskStreams.end();tsi++)
    {
      TaskStream* ts = *tsi;
      for(int i=0;i<4;i++)
      {
        if (ts->getTask(i).getType()!=TaskTypes::None)
        {
          if (ts->getTask(i).getResources().getGas()>0 && finalCounts[Broodwar->self()->getRace().getRefinery()] == 0)
          {
            finalCounts[Broodwar->self()->getRace().getRefinery()] = 1;
            TaskStream* nts = new TaskStream(Task(Broodwar->self()->getRace().getRefinery()));
            TheMacroManager->insertTaskStreamBelow(nts,ts);
            nts->attach(new BasicWorkerFinder(),true);
            nts->attach(BasicTaskExecutor::getInstance(),false);
            nts->attach(new TerminateIfEmpty(),true);
            nts->attach(BFSBuildingPlacer::getInstance(),false);
          }
          map<UnitType, int> requiredUnits = ts->getTask(i).getRequiredUnits();
          for each(pair<UnitType,int> r in requiredUnits)
          {
            if (r.first == UnitTypes::Zerg_Larva) continue;
            if (r.first.mineralPrice()>0 && TheResourceRates->getGatherRate().getMinerals() == 0) continue;
            if (r.first.gasPrice()>0 && TheResourceRates->getGatherRate().getGas() == 0) continue;
            if (r.first.isAddon()) continue;
            if (finalCounts[r.first]==0)
            {
              finalCounts[r.first]=1;
              TaskStream* nts = new TaskStream(Task(r.first));
              TheMacroManager->insertTaskStreamBelow(nts,ts);
              nts->attach(new BasicWorkerFinder(),true);
              nts->attach(BasicTaskExecutor::getInstance(),false);
              nts->attach(new TerminateIfEmpty(),true);
              nts->attach(BFSBuildingPlacer::getInstance(),false);
            }
          }
        }
      }
    }
  }
}