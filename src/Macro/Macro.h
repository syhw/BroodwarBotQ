#pragma once
#include <Utils/CSingleton.h>
#include "Macro/ResourceRates.h"
//#include "Macro/SupplyManager.h"
#include "Macro/InformationManager.h"
#include "Macro/BorderManager.h"
#include "Macro/BasesManager.h"
#include "Macro/UnitGroupManager.h"
#include "Macro/WorkerManager.h"
#include "Macro/ReservedMap.h"
#include "Macro/Builder.h"
#include "Macro/Producer.h"

class Macro: public CSingleton<Macro>
{
    friend class CSingleton<Macro>;
	Macro();
	~Macro();
    Arbitrator::Arbitrator<BWAPI::Unit*,double> arbitrator;
	int expands;
	int addedGates;

public:
	int reservedMinerals;
	int reservedGas;
	void buildOrderAdd(BWAPI::UnitType type);
	void techAdd(BWAPI::TechType type);
	void upgradeAdd(BWAPI::UpgradeType type);
	void update();
	void onUnitDiscover(BWAPI::Unit* unit);
	void onUnitEvade(BWAPI::Unit* unit);
	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);
	void onUnitMorph(BWAPI::Unit* unit);
	void onUnitRenegade(BWAPI::Unit* unit);
};