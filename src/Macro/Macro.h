#pragma once
#include <CSingleton.h>
#include <BasicTaskExecutor.h>
#include <SpiralBuildingPlacer.h>
#include <BFSBuildingPlacer.h>
#include <UnitPump.h>
#include <TerminateIfWorkerLost.h>
#include <TerminateIfEmpty.h>
#include <BasicWorkerFinder.h>
#include <UnitCompositionProducer.h>
#include <MacroManager.h>
#include <ResourceRates.h>
#include <MacroSupplyManager.h>
#include <MacroDependencyResolver.h>
#include <InformationManager.h>
#include <BorderManager.h>
#include <MacroBaseManager.h>
#include <UnitGroupManager.h>
#include <MacroWorkerManager.h>
#include <ReservedMap.h>

class Macro: public CSingleton<Macro>
{
    friend class CSingleton<Macro>;
	Macro();
	~Macro();
    Arbitrator::Arbitrator<BWAPI::Unit*,double> arbitrator;
public:
	void buildOrderAdd(BWAPI::UnitType type);
	void techAdd(BWAPI::TechType type);
	void upgradeAdd(BWAPI::UpgradeType type);
	void update();
	void onUnitDiscover(BWAPI::Unit* unit);
	void onUnitEvade(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);
	void onUnitMorph(BWAPI::Unit* unit);
	void onUnitRenegade(BWAPI::Unit* unit);
};