#include <PrecompiledHeader.h>
#include "Macro.h"

using namespace BWAPI;

UnitCompositionProducer* infantryProducer = NULL; // TODO remove

Macro::Macro()
: expands(0)
, addedGates(0)
{
	TheArbitrator = & arbitrator;
	MacroManager::create();
	MacroSupplyManager::create();
	MacroDependencyResolver::create();
	ResourceRates::create();
	InformationManager::create();
	BorderManager::create();
	MacroBaseManager::create();
	UnitGroupManager::create();
	MacroWorkerManager::create();
	ReservedMap::create();

	TaskStream* ts = new TaskStream();
	TheMacroManager->taskStreams.push_back(ts);
	Unit* worker = NULL;
	for each(Unit* u in Broodwar->self()->getUnits())
	{
		if (u->getType().isResourceDepot())
			worker = u;
	}
	ts->setWorker(worker);
	ts->attach(BasicTaskExecutor::getInstance(),false);
	ts->attach(new UnitPump(Broodwar->self()->getRace().getWorker()),true);
	ts->attach(new TerminateIfWorkerLost(),true);
	if (Broodwar->self()->getRace() == Races::Protoss)
	{
		buildOrderAdd(UnitTypes::Protoss_Gateway);
		buildOrderAdd(UnitTypes::Protoss_Cybernetics_Core);
		buildOrderAdd(UnitTypes::Protoss_Assimilator);
		buildOrderAdd(UnitTypes::Protoss_Gateway);
		addedGates = 2;
		infantryProducer = new UnitCompositionProducer(UnitTypes::Protoss_Gateway);
		infantryProducer->setUnitWeight(UnitTypes::Protoss_Dragoon, 3.0);
		infantryProducer->setUnitWeight(UnitTypes::Protoss_Zealot, 1.0);
		upgradeAdd(UpgradeTypes::Singularity_Charge);
	}
	else if (Broodwar->self()->getRace() == Races::Terran)
	{
		buildOrderAdd(UnitTypes::Terran_Supply_Depot);
		buildOrderAdd(UnitTypes::Terran_Barracks);
		buildOrderAdd(UnitTypes::Terran_Refinery);
		buildOrderAdd(UnitTypes::Terran_Academy);
		buildOrderAdd(UnitTypes::Terran_Barracks);
		buildOrderAdd(UnitTypes::Terran_Barracks);
		infantryProducer = new UnitCompositionProducer(UnitTypes::Terran_Barracks);
		infantryProducer->setUnitWeight(UnitTypes::Terran_Marine, 3.0);
		infantryProducer->setUnitWeight(UnitTypes::Terran_Medic, 1.0);
		techAdd(TechTypes::Stim_Packs);
	}
	{
		Broodwar->printf("Race not implemented");
	}
}

Macro::~Macro()
{
	MacroManager::destroy();
	MacroSupplyManager::destroy();
	MacroDependencyResolver::destroy();
	ResourceRates::destroy();
	InformationManager::destroy();
	BorderManager::destroy();
	MacroBaseManager::destroy();
	UnitGroupManager::destroy();
	MacroWorkerManager::destroy();
	ReservedMap::destroy();
}

void Macro::buildOrderAdd(UnitType type)
{
	TaskStream* ts = new TaskStream(Task(type));
	TheMacroManager->taskStreams.push_back(ts);
	ts->attach(new BasicWorkerFinder(),true);
	ts->attach(BasicTaskExecutor::getInstance(),false);
	ts->attach(new TerminateIfEmpty(),true);
	ts->attach(SimCityBuildingPlacer::getInstance(),false);
}

void Macro::techAdd(TechType type)
{
	TaskStream* ts = new TaskStream(Task(type));
	TheMacroManager->taskStreams.push_back(ts);
	ts->attach(new BasicWorkerFinder(),true);
	ts->attach(BasicTaskExecutor::getInstance(),false);
	ts->attach(new TerminateIfEmpty(),true);
	ts->attach(SimCityBuildingPlacer::getInstance(),false);
}

void Macro::upgradeAdd(UpgradeType type)
{
	TaskStream* ts = new TaskStream(Task(type));
	TheMacroManager->taskStreams.push_back(ts);
	ts->attach(new BasicWorkerFinder(),true);
	ts->attach(BasicTaskExecutor::getInstance(),false);
	ts->attach(new TerminateIfEmpty(),true);
	ts->attach(SimCityBuildingPlacer::getInstance(),false);
}

void Macro::update()
{
    if (infantryProducer)
		infantryProducer->update();
	TheMacroSupplyManager->update();
	TheMacroManager->update();
	TheResourceRates->update();
	TheBorderManager->update();
	TheMacroBaseManager->update();
	TheMacroWorkerManager->update();
	TheArbitrator->update();

	if (!expands && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 2)
	{
		++expands;
		TheMacroBaseManager->expandWhenPossible();
		//buildOrderAdd(UnitTypes::Protoss_Assimilator);
	}
	else if (expands == 1 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observer))
	{
		++expands;
		TheMacroBaseManager->expandWhenPossible();
		//buildOrderAdd(UnitTypes::Protoss_Assimilator);
	}
	if (addedGates < 3 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 1)
	{
		buildOrderAdd(UnitTypes::Protoss_Gateway);
		++addedGates;
	}
	
	/*if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) > 1)
	{
		buildOrderAdd(UnitTypes::Protoss_Observatory);
		upgradeAdd(UpgradeTypes::Protoss_Ground_Weapons);
		upgradeAdd(UpgradeTypes::Protoss_Ground_Armor);
	}*/
	/*
	if (Broodwar->self()->minerals() > 500 && addedGates < 8)
	{
        ++addedGates;
		buildOrderAdd(UnitTypes::Protoss_Gateway);
	}*/
}

void Macro::onUnitDiscover(BWAPI::Unit* unit)
{
  TheInformationManager->onUnitDiscover(unit);
  TheUnitGroupManager->onUnitDiscover(unit);
}

void Macro::onUnitEvade(BWAPI::Unit* unit)
{
  TheInformationManager->onUnitEvade(unit);
  TheUnitGroupManager->onUnitEvade(unit);
}

void Macro::onUnitDestroy(BWAPI::Unit* unit)
{
  TheArbitrator->onRemoveObject(unit);
  TheInformationManager->onUnitDestroy(unit);
  TheMacroBaseManager->onUnitDestroy(unit);
  TheMacroWorkerManager->onRemoveUnit(unit);
}

void Macro::onUnitMorph(BWAPI::Unit* unit)
{
  TheUnitGroupManager->onUnitMorph(unit);
}

void Macro::onUnitRenegade(BWAPI::Unit* unit)
{
  TheUnitGroupManager->onUnitRenegade(unit);
}
