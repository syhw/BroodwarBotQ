#include <PrecompiledHeader.h>
#include "Macro/MacroSupplyManager.h"
using namespace BWAPI;
using namespace std;

set<UnitType> factoryTypes;
SupplyManager* TheSupplyManager = NULL;

SupplyManager* SupplyManager::create()
{
	if (TheSupplyManager)
		return TheSupplyManager;
	TheSupplyManager = new SupplyManager();
	return TheSupplyManager;
}

void SupplyManager::destroy()
{
	if (TheSupplyManager)
		delete TheSupplyManager;
}

SupplyManager::SupplyManager()
{
	TheSupplyManager = this;
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

SupplyManager::~SupplyManager()
{
	TheSupplyManager = NULL;
}

void SupplyManager::update()
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
						ts->attach(SimCityBuildingPlacer::getInstance(),false);
						// SimCityBuildingPlacer::getInstance()->setBuildDistance(ts,4); // TODO redo
					}
					else
						ts->attach(SimCityBuildingPlacer::getInstance(),false);

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
							ts->attach(SimCityBuildingPlacer::getInstance(),false);
						else
							ts->attach(SimCityBuildingPlacer::getInstance(),false);
					}
				}
			}
		}
	}
}