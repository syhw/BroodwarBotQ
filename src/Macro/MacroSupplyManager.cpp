#include <PrecompiledHeader.h>
#include "Macro/MacroSupplyManager.h"
#include "Macro/MacroManager.h"
#include "Macro/ResourceRates.h"
#include "Macro/BasicTaskExecutor.h"
#include "Macro/SpiralBuildingPlacer.h"
#include "Macro/BFSBuildingPlacer.h"
#include "Macro/PylonBuildingPlacer.h"
#include "Macro/UnitPump.h"
#include "Macro/TerminateIfWorkerLost.h"
#include "Macro/TerminateIfEmpty.h"
#include "Macro/BasicWorkerFinder.h"
#include "Macro/UnitCompositionProducer.h"
using namespace BWAPI;
using namespace std;
set<UnitType> factoryTypes;
MacroSupplyManager* TheMacroSupplyManager = NULL;
MacroSupplyManager* MacroSupplyManager::create()
{
  if (TheMacroSupplyManager) return TheMacroSupplyManager;
  return new MacroSupplyManager();
}
void MacroSupplyManager::destroy()
{
  if (TheMacroSupplyManager)
    delete TheMacroSupplyManager;
}
MacroSupplyManager::MacroSupplyManager()
{
  TheMacroSupplyManager=this;
  for each(UnitType t in UnitTypes::allUnitTypes())
  {
    if (t.canProduce())
    {
      factoryTypes.insert(t);
    }
  }
  initialSupplyTotal = Broodwar->self()->supplyTotal();
  initialSupplyProviderCount = Broodwar->self()->completedUnitCount(BWAPI::Broodwar->self()->getRace().getSupplyProvider());
  lastFrameCheck=0;
}
MacroSupplyManager::~MacroSupplyManager()
{
  TheMacroSupplyManager = NULL;
}
void MacroSupplyManager::update()
{
  if (TheResourceRates->getGatherRate().getMinerals()>0)
  {
    if (Broodwar->getFrameCount()>lastFrameCheck+25)
    {
      lastFrameCheck=Broodwar->getFrameCount();
      if (TheMacroManager->uctl.getFinalCount(BWAPI::Broodwar->self()->getRace().getSupplyProvider())==initialSupplyProviderCount)
      {
        if (Broodwar->self()->supplyUsed()>=initialSupplyTotal-2*(1-initialSupplyProviderCount))
        {
          Task s(Broodwar->self()->getRace().getSupplyProvider());
          TaskStream* ts = new TaskStream(s);
          TheMacroManager->taskStreams.push_front(ts);
          ts->attach(new BasicWorkerFinder(),true);
          ts->attach(BasicTaskExecutor::getInstance(),false);
          ts->attach(new TerminateIfEmpty(),true);
          if (Broodwar->self()->getRace()==Races::Protoss)
          {
            ts->attach(PylonBuildingPlacer::getInstance(),false);
            PylonBuildingPlacer::getInstance()->setBuildDistance(ts,4);
          }
          else
            ts->attach(BFSBuildingPlacer::getInstance(),false);

        }
      }
      else
      {
        int supplyBuildTime = BWAPI::Broodwar->self()->getRace().getSupplyProvider().buildTime();
        if (TheMacroManager->rtl.getAvailableResourcesAtTime(Broodwar->getFrameCount()+supplyBuildTime*3).getSupply()<=0)
        {
          if (TheMacroManager->rtl.getFinalSupplyTotal()<400)
          {
            Task s(Broodwar->self()->getRace().getSupplyProvider());
            int frame = TheMacroManager->rtl.getFirstTimeWhenSupplyIsNoGreaterThan(0);
            s.setEarliestStartTime(frame-334-supplyBuildTime);
            TaskStream* ts = new TaskStream(s);
            TheMacroManager->taskStreams.push_front(ts);
            ts->attach(new BasicWorkerFinder(),true);
            ts->attach(BasicTaskExecutor::getInstance(),false);
            ts->attach(new TerminateIfEmpty(),true);
            if (Broodwar->self()->getRace()==Races::Protoss)
              ts->attach(PylonBuildingPlacer::getInstance(),false);
            else
              ts->attach(BFSBuildingPlacer::getInstance(),false);
          }
        }
      }
    }
  }
}