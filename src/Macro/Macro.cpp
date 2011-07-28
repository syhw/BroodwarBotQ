#include <PrecompiledHeader.h>
#include "Macro.h"
#include "BWSAL.h"

using namespace BWAPI;

Macro::Macro()
: expands(0)
, addedGates(0)
, reservedMinerals(0)
, reservedGas(0)
{
	TheArbitrator = & arbitrator;
	ReservedMap::create();
	Builder::create();
	Producer::create();
	//Upgrader::create();
	//SupplyManager::create();
	ResourceRates::create();
	InformationManager::create();
	BorderManager::create();
	BasesManager::create();
	UnitGroupManager::create();
	WorkerManager::create();
}

Macro::~Macro()
{
	Producer::destroy();
	//Upgrader::destroy();
	//SupplyManager::destroy();
	ResourceRates::destroy();
	InformationManager::destroy();
	BorderManager::destroy();
	BasesManager::destroy();
	UnitGroupManager::destroy();
	WorkerManager::destroy();
	ReservedMap::destroy();
	Builder::destroy();
}


void Macro::init()
{
	if (Broodwar->self()->getRace() == Races::Protoss)
	{
		TheWorkerManager->enableAutoBuild();
		if (Broodwar->enemy()->getRace() == Races::Zerg)
		{
		}
		else if (Broodwar->enemy()->getRace() == Races::Terran)
		{
		}
		else
		{
			TheBuilder->buildOrder(UnitTypes::Protoss_Pylon, 8);
			TheBuilder->buildOrder(UnitTypes::Protoss_Gateway, 10);
			TheBasesManager->setFirstGasPop(12);
			TheBuilder->buildOrder(UnitTypes::Protoss_Pylon, 16);
			TheBuilder->buildOrder(UnitTypes::Protoss_Cybernetics_Core, 18);
			//TheUpgrader->upgrade(UpgradeTypes::Singularity_Charge);
		}
	}
	else
	{
		Broodwar->printf("Race not implemented");
	}
}

void Macro::update()
{
#ifdef __DEBUG__
	Broodwar->drawTextScreen(130, 38, "\x11 rM: %d, rG: %d", reservedMinerals, reservedGas);
#endif
	if (reservedMinerals < 0) // safety
		reservedMinerals = 0;
	if (reservedGas < 0)
		reservedGas = 0;

	// TODO PRODUCTION
	//TheUpgrader->update();
	TheProducer->update();

	//TheSupplyManager->update();

	TheResourceRates->update();

	TheBorderManager->update();

	TheBasesManager->update();

	TheWorkerManager->update();

	TheBuilder->update(); // last update which moves an unit that should be done

	TheArbitrator->update();

	if (!expands && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 2)
	{
		++expands;
		TheBasesManager->expand();
		//buildOrderAdd(UnitTypes::Protoss_Assimilator);
	}
	else if (expands == 1 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observer))
	{
		++expands;
		TheBasesManager->expand();
		//buildOrderAdd(UnitTypes::Protoss_Assimilator);
	}
	if (addedGates < 3 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 1)
	{
		//buildOrderAdd(UnitTypes::Protoss_Gateway);
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

void Macro::onUnitCreate(BWAPI::Unit* unit)
{
	TheProducer->onUnitCreate(unit);
	TheBuilder->onUnitCreate(unit);
	//TheUpgrader->onUnitCreate(unit);

	if (unit->getPlayer() == Broodwar->self())
	{
		if (unit->getType() == UnitTypes::Protoss_Cybernetics_Core && Broodwar->getFrameCount() < 12000)
		{
			/// Built the (first) core
			if (Broodwar->enemy()->getRace() == Races::Zerg)
			{
				TheProducer->produce(6, UnitTypes::Protoss_Zealot, 49, 2);
				TheProducer->produce(16, UnitTypes::Protoss_Dragoon, 50);
			}
			else if (Broodwar->enemy()->getRace() == Races::Terran)
			{
				TheProducer->produce(16, UnitTypes::Protoss_Dragoon, 50);
			}
			else
			{
				TheProducer->produce(16, UnitTypes::Protoss_Dragoon, 50);
			}
		}
		else if (unit->getType() == UnitTypes::Protoss_Templar_Archives)
		{
			/// Built templar archives
			TheProducer->produce(10, UnitTypes::Protoss_High_Templar, 54, 3);
		}
		else if (unit->getType() == UnitTypes::Protoss_Gateway)
		{
			/// 1st Gateway (in a "< 19 supply" sense)
			if (Broodwar->self()->supplyUsed() < 40)
				TheProducer->produce(2, UnitTypes::Protoss_Zealot, 50);
			/// 3rd Gateway
			if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) > 2
				&& !Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Forge))
				TheBuilder->build(UnitTypes::Protoss_Forge);
		}
	}
}

void Macro::onUnitDestroy(BWAPI::Unit* unit)
{
	TheArbitrator->onRemoveObject(unit);
	TheInformationManager->onUnitDestroy(unit);
	TheBasesManager->onUnitDestroy(unit);
	TheWorkerManager->onUnitDestroy(unit);
	TheBuilder->onUnitDestroy(unit);
	TheProducer->onUnitDestroy(unit);
	//TheUpgrader->onUnitDestroy(unit);
}

void Macro::onUnitMorph(BWAPI::Unit* unit)
{
	TheUnitGroupManager->onUnitMorph(unit);
}

void Macro::onUnitRenegade(BWAPI::Unit* unit)
{
	TheUnitGroupManager->onUnitRenegade(unit);
}
