#pragma once
#include <Utils/CSingleton.h>
#include "Macro/BasicTaskExecutor.h"
#include "Macro/SpiralBuildingPlacer.h"
#include "Macro/BFSBuildingPlacer.h"
#include "Macro/UnitPump.h"
#include "Macro/TerminateIfWorkerLost.h"
#include "Macro/TerminateIfEmpty.h"
#include "Macro/BasicWorkerFinder.h"
#include "Macro/UnitCompositionProducer.h"
#include "Macro/MacroManager.h"
#include "Macro/ResourceRates.h"
#include "Macro/MacroSupplyManager.h"
#include "Macro/MacroDependencyResolver.h"
#include "Macro/InformationManager.h"
#include "Macro/BorderManager.h"
#include "Macro/MacroBaseManager.h"
#include "Macro/UnitGroupManager.h"
#include "Macro/MacroWorkerManager.h"
#include "Macro/ReservedMap.h"

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