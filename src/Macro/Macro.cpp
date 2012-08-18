#include <PrecompiledHeader.h>
#include "Macro.h"
#include "BWSAL.h"
#include "Intelligence/ETechEstimator.h"
#include "Intelligence/EUnitsFilter.h"
#include "Intelligence/Intelligence.h"
#include "Macro/Producer.h"

using namespace BWAPI;

inline bool dontHave(BWAPI::UnitType ut)
{
    return !Broodwar->self()->completedUnitCount(ut) && !Broodwar->self()->incompleteUnitCount(ut);
}

inline bool wontHave(BWAPI::UnitType ut)
{
	return dontHave(ut) && !TheBuilder->numberInFutureTasks(ut);
}

inline int sumWillHave(BWAPI::UnitType ut)
{
	return Broodwar->self()->completedUnitCount(ut) + TheBuilder->willBuild(ut);
}

Macro::Macro()
: expands(0)
, addedGates(0)
, reservedMinerals(0)
, reservedGas(0)
, stormFirst(false)
, reaverFirst(false)
{
	TheArbitrator = & arbitrator;
	ReservedMap::create();
	Builder::create();
	Producer::create();
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
			/*TheBuilder->buildOrder(UnitTypes::Protoss_Pylon, 8);
			TheBuilder->buildOrder(UnitTypes::Protoss_Nexus, 15);
			TheBuilder->buildOrder(UnitTypes::Protoss_Forge, 16);
			TheBuilder->buildOrder(UnitTypes::Protoss_Pylon, 16);
			TheBuilder->buildOrder(UnitTypes::Protoss_Photon_Cannon, 18);
			TheBuilder->buildOrder(UnitTypes::Protoss_Photon_Cannon, 18);*/
			TheBuilder->buildOrder(UnitTypes::Protoss_Pylon, 8);
			TheBuilder->buildOrder(UnitTypes::Protoss_Gateway, 11);
			TheBasesManager->setFirstGasPop(12);
			TheBuilder->buildOrder(UnitTypes::Protoss_Cybernetics_Core, 15);
			TheBuilder->buildOrder(UnitTypes::Protoss_Pylon, 16);
		}
		else if (Broodwar->enemy()->getRace() == Races::Terran)
		{
			TheBuilder->buildOrder(UnitTypes::Protoss_Pylon, 8);
			TheBuilder->buildOrder(UnitTypes::Protoss_Gateway, 11);
			TheBasesManager->setFirstGasPop(12);
			TheBuilder->buildOrder(UnitTypes::Protoss_Cybernetics_Core, 15);
			TheBuilder->buildOrder(UnitTypes::Protoss_Gateway, 16);
			TheBuilder->buildOrder(UnitTypes::Protoss_Pylon, 17);
		}
		else
		{
			TheBuilder->buildOrder(UnitTypes::Protoss_Pylon, 8);
			TheBuilder->buildOrder(UnitTypes::Protoss_Gateway, 11);
			TheBasesManager->setFirstGasPop(12);
			TheBuilder->buildOrder(UnitTypes::Protoss_Cybernetics_Core, 15);
			TheBuilder->buildOrder(UnitTypes::Protoss_Pylon, 16);
		}
	}
	else
	{
		Broodwar->printf("Race not implemented");
	}
}

void Macro::update()
{
#ifdef __MACRO_DEBUG__
	Broodwar->drawTextScreen(130, 38, "\x11 rM: %d, rG: %d", reservedMinerals, reservedGas);
#endif
	if (reservedMinerals < 0) // safety
		reservedMinerals = 0;
	if (reservedGas < 0)
		reservedGas = 0;

	TheProducer->update();

	TheResourceRates->update();

	TheBorderManager->update();

	TheBasesManager->update();

	TheWorkerManager->update();

	TheBuilder->update(); // last update which moves an unit that should be done

	TheArbitrator->update();

	
	/// TODO REMOVE THAT
	if (!expands && (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 12 || Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) > 2))
	{
		expand();
	}

	
	if (Intelligence::Instance().enemyRush && Broodwar->getFrameCount() < 6*24*60)
		TheWorkerManager->setWorkersPerGas(2);
	else
		TheWorkerManager->setWorkersPerGas(3);
	if (TheWorkerManager->numberWorkers() < 14 || Broodwar->self()->minerals() < 400 && Broodwar->self()->gas() > 800)
		TheWorkerManager->setWorkersPerGas(1);
	else
		TheWorkerManager->setWorkersPerGas(3);

	if (Broodwar->getFrameCount() > 5*24*60) // failsafes post very early rushes
	{
		if (!(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core))
			&& !TheBuilder->willBuild(UnitTypes::Protoss_Cybernetics_Core))
			TheBuilder->build(UnitTypes::Protoss_Cybernetics_Core, TilePositions::None, true);
		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Pylon) < 2
			&& !TheBuilder->willBuild(UnitTypes::Protoss_Pylon))
			TheBuilder->build(UnitTypes::Protoss_Pylon, TilePositions::None, true);
	}


	if (!expands)
	{
		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 4
			//|| Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) + Broodwar->self()->incompleteUnitCount(UnitTypes::Protoss_Gateway) >= 2)
			&& !Intelligence::Instance().enemyRush)
		{
			expand();
		}
		else if (Broodwar->self()->minerals() > 400)
			expand();
	}
	else if (expands == 1)
	{
		if (!(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core))
			&& wontHave(UnitTypes::Protoss_Cybernetics_Core))
			TheBuilder->build(UnitTypes::Protoss_Cybernetics_Core, TilePositions::None, true);

		if (wontHave(UnitTypes::Protoss_Robotics_Facility))
			TheBuilder->build(UnitTypes::Protoss_Robotics_Facility);

		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Photon_Cannon)
			+ TheBuilder->willBuild(UnitTypes::Protoss_Photon_Cannon) < 5)
			TheBuilder->build(UnitTypes::Protoss_Photon_Cannon);

		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observer)
			///&& (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver > 1) || Broodwar->self()->completedUnitCount(UnitTypes::Protoss_High_Templar) > 2)
			)
		{
			expand();
			TheProducer->produceAlways(10, UnitTypes::Protoss_Zealot, 3);
		}
	}
	else if (expands > 1)
	{
		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Shuttle))
			TheProducer->researchUpgrade(UpgradeTypes::Gravitic_Drive);
		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_High_Templar) > 7)
		{
			TheProducer->researchTech(TechTypes::Maelstrom);
			TheProducer->produce(2, UnitTypes::Protoss_Dark_Archon, 60, 2);
			TheProducer->produceAlways(2, UnitTypes::Protoss_Dark_Archon, 6);
		}
		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Zealot) > 7)
		{
			TheProducer->researchUpgrade(UpgradeTypes::Leg_Enhancements);
		}
		if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Protoss_Ground_Armor) < 2)
		{
			TheProducer->researchUpgrade(UpgradeTypes::Protoss_Ground_Armor);
			TheProducer->researchUpgrade(UpgradeTypes::Protoss_Ground_Weapons);
		}
		if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Protoss_Ground_Weapons) < 1)
			TheProducer->researchUpgrade(UpgradeTypes::Protoss_Ground_Weapons);
	}
	else if (expands > 2)
	{
		if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Protoss_Ground_Armor) < 3)
		{
			TheProducer->researchUpgrade(UpgradeTypes::Protoss_Ground_Armor);
			TheProducer->researchUpgrade(UpgradeTypes::Protoss_Ground_Weapons);
		}
	}
	if (Broodwar->self()->minerals() > 250 
		&& Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Support_Bay))
	{
#ifdef __REAVER_FIRST__
		if (!Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Shuttle))
			TheProducer->produce(1, UnitTypes::Protoss_Shuttle, 90);
#endif
		if (Broodwar->self()->gas() > 150 && !Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver))
			TheProducer->produce(1, UnitTypes::Protoss_Reaver, 95);
	}
#ifdef __REAVER_FIRST__
	reaverFirst = true;// TODO REMOVE DEBUG
#endif
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
/*
Terran openings, in order (in the vector):
 - Bio            ==> Storm
 - TwoFactory     ==> Obs
 - VultureHarass  ==> Obs
 - SiegeExpand    ==> Reaver/Drop
 - Standard       ==> Reaver/Drop
 - FastDropship   ==> Reaver/Drop
 - Unknown        ==> Obs

Protoss openings, in order (in the vector):
 - FastLegs       ==> Obs/Reaver
 - FastDT         ==> Cannons/Obs
 - FastObs        ==> Reaver/Drop
 - ReaverDrop     ==> Reaver/Drop
 - Carrier        ==> "lol"
 - FastExpand     ==> Expand/Reave/Drop
 - Unknown        ==> Obs

Zerg openings, in order (in the vector):
 - TwoHatchMuta   ==> Storm
 - ThreeHatchMuta ==> Storm
 - HydraRush      ==> Goons/ZLegs/Storm/Obs
 - Standard       ==> ZLegs/Storm/Goons
 - HydraMass      ==> ZLegs/Storm
 - Lurker         ==> Obs/Reaver/Drop/Storm
 - Unknown        ==> Storm
 */
	TheProducer->onUnitCreate(unit);
	TheBuilder->onUnitCreate(unit);

	if (unit->getPlayer() == Broodwar->self())
	{
		UnitType ut =unit->getType();
		Race er = Broodwar->enemy()->getRace();

		/////////////////// T1 ///////////////////
		// Core->Singulary, Zealots vs Zerg and always Goons (against all)

		if (ut == UnitTypes::Protoss_Cybernetics_Core && Broodwar->getFrameCount() < 16000)
		{
			/// Built the (first) core
			if (!Intelligence::Instance().enemyRush)
				TheProducer->researchUpgrade(UpgradeTypes::Singularity_Charge);
			if (er == Races::Zerg)
			{
				TheProducer->produce(5, UnitTypes::Protoss_Zealot, 49, 2);
				TheProducer->produce(16, UnitTypes::Protoss_Dragoon, 50);
				TheProducer->produceAlways(32, UnitTypes::Protoss_Dragoon);
				TheProducer->produceAlways(8, UnitTypes::Protoss_Zealot, 2);
			}
			else if (er == Races::Terran)
			{
				TheProducer->produce(2, UnitTypes::Protoss_Zealot, 49, 2);
				TheProducer->produce(16, UnitTypes::Protoss_Dragoon, 50);
				TheProducer->produceAlways(32, UnitTypes::Protoss_Dragoon);
				TheProducer->produceAlways(8, UnitTypes::Protoss_Zealot, 2);
			}
			else
			{
				TheProducer->produce(2, UnitTypes::Protoss_Zealot, 49, 2);
				TheProducer->produce(16, UnitTypes::Protoss_Dragoon, 50);
				TheProducer->produceAlways(32, UnitTypes::Protoss_Dragoon);
				TheProducer->produceAlways(8, UnitTypes::Protoss_Zealot, 2);
			}
		}
		else if (ut == UnitTypes::Protoss_Gateway)
		{
			/// 1st Gateway (in a "< 19 supply" sense)
			if (Broodwar->self()->supplyUsed() < 40)
			{
				if (er == Races::Zerg)
					TheProducer->produce(3, UnitTypes::Protoss_Zealot, 50);
			}
			/// 3rd Gateway and oponent not Terran -> forge (for cannons against mutas and DTs, and for +1 attack against Z)
			if (er != Races::Terran
				&& Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) > 2
				&& wontHave(UnitTypes::Protoss_Forge))
			{
				TheBuilder->build(UnitTypes::Protoss_Forge);
			}
			if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) > 3
				&& !(Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge)))
				TheProducer->researchUpgrade(UpgradeTypes::Singularity_Charge);
		}

		/////////////////// T2+ ///////////////////

		else if (ut == UnitTypes::Protoss_Forge)
		{
			if (!Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Forge))
			{
				if (er == Races::Zerg)
					TheProducer->researchUpgrade(UpgradeTypes::Protoss_Ground_Weapons);
			}
			else
			{
				TheProducer->researchUpgrade(UpgradeTypes::Protoss_Ground_Weapons);
				TheProducer->researchUpgrade(UpgradeTypes::Protoss_Ground_Armor);
			}
		}
		else if (ut == UnitTypes::Protoss_Nexus)
		{
			if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) == 2) // it's the first expand then (1 complete + 1 incomplete)
			{
				if (!(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core))
					&& !(TheBuilder->willBuild(UnitTypes::Protoss_Cybernetics_Core)))
				{
					TheBuilder->build(UnitTypes::Protoss_Cybernetics_Core, TilePositions::None, true);
				}
				if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) < 2
					&& !(TheBuilder->willBuild(UnitTypes::Protoss_Gateway)))
				{
					TheBuilder->build(UnitTypes::Protoss_Gateway, TilePositions::None, true);
				}
				if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Pylon) < 3
					&& !(TheBuilder->willBuild(UnitTypes::Protoss_Pylon)))
				{
					TheBuilder->build(UnitTypes::Protoss_Pylon, TilePositions::None, true);
				}
					
				if (!reaverFirst && !stormFirst && wontHave(UnitTypes::Protoss_Observatory))
				{
					if (wontHave(UnitTypes::Protoss_Robotics_Facility))
						TheBuilder->build(UnitTypes::Protoss_Robotics_Facility);
					else if (wontHave(UnitTypes::Protoss_Observatory))
						TheBuilder->build(UnitTypes::Protoss_Observatory);
				}

				if (reaverFirst)
				{
					if (wontHave(UnitTypes::Protoss_Robotics_Facility))
						TheBuilder->build(UnitTypes::Protoss_Robotics_Facility);
					else if (wontHave(UnitTypes::Protoss_Robotics_Support_Bay))
						TheBuilder->build(UnitTypes::Protoss_Robotics_Facility);
				}
				//else 
				if (stormFirst && wontHave(UnitTypes::Protoss_Citadel_of_Adun))
				    TheBuilder->build(UnitTypes::Protoss_Citadel_of_Adun);

				if (wontHave(UnitTypes::Protoss_Forge))
					TheBuilder->build(UnitTypes::Protoss_Forge);
			}
			if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) == 3) // B3
			{
				if (sumWillHave(UnitTypes::Protoss_Forge) < 2)
					TheBuilder->build(UnitTypes::Protoss_Forge);
				if (wontHave(UnitTypes::Protoss_Templar_Archives))
					TheBuilder->build(UnitTypes::Protoss_Templar_Archives);
				if (wontHave(UnitTypes::Protoss_Robotics_Facility))
					TheBuilder->build(UnitTypes::Protoss_Robotics_Facility);
			}
			if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) == 4) // B4
			{
				TheProducer->researchUpgrade(UpgradeTypes::Reaver_Capacity);
				TheProducer->researchUpgrade(UpgradeTypes::Scarab_Damage);
				if (wontHave(UnitTypes::Protoss_Stargate))
					TheBuilder->build(UnitTypes::Protoss_Stargate);
				int missingCannons = 3 + Macro::Instance().expands * 3
					- (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Photon_Cannon) + TheBuilder->willBuild(UnitTypes::Protoss_Photon_Cannon));
				while (missingCannons > 0)
				{
					--missingCannons;
					TheBuilder->build(UnitTypes::Protoss_Photon_Cannon);
				}
			}
		}
		else if (ut == UnitTypes::Protoss_Citadel_of_Adun)
		{
			if (wontHave(UnitTypes::Protoss_Templar_Archives))
				TheBuilder->build(UnitTypes::Protoss_Templar_Archives);
		}
		else if (ut == UnitTypes::Protoss_Robotics_Facility)
		{
			if (reaverFirst)
			{
				if (wontHave(UnitTypes::Protoss_Robotics_Support_Bay))
					TheBuilder->build(UnitTypes::Protoss_Robotics_Support_Bay);
#ifdef __REAVER_FIRST__
				TheProducer->produce(1, UnitTypes::Protoss_Shuttle, 95);
				TheProducer->produceAlways(1, UnitTypes::Protoss_Shuttle, 3);
#endif
			}
			else
			{
				if (wontHave(UnitTypes::Protoss_Observatory))
					TheBuilder->build(UnitTypes::Protoss_Observatory);
			}
		}

		/////////////////// T3 ///////////////////

		else if (ut == UnitTypes::Protoss_Templar_Archives)
		{
			/// Built templar archives
			TheProducer->researchTech(TechTypes::Psionic_Storm);
			TheProducer->researchUpgrade(UpgradeTypes::Khaydarin_Amulet);
			TheProducer->produce(8, UnitTypes::Protoss_High_Templar, 56, 3);
			TheProducer->produceAlways(12, UnitTypes::Protoss_High_Templar, 3);
			if (er == Races::Zerg)
				TheProducer->produce(2, UnitTypes::Protoss_Archon, 50, 3);
		}
		else if (ut == UnitTypes::Protoss_Observatory)
		{
			TheProducer->produce(2, UnitTypes::Protoss_Observer, 60, 2);
		}
		else if (ut == UnitTypes::Protoss_Robotics_Support_Bay)
		{
			TheProducer->produce(1, UnitTypes::Protoss_Reaver, 95);
#ifdef __REAVER_FIRST__
			if (!Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Shuttle))
				TheProducer->produce(1, UnitTypes::Protoss_Shuttle, 95);
#endif
			TheProducer->produceAlways(4, UnitTypes::Protoss_Reaver, 5);
		}
		else if (ut == UnitTypes::Protoss_Observer)
		{
			if (!stormFirst && wontHave(UnitTypes::Protoss_Robotics_Support_Bay))
				TheBuilder->build(UnitTypes::Protoss_Robotics_Support_Bay);
		}
		else if (ut == UnitTypes::Protoss_Reaver)
		{
			TheProducer->researchUpgrade(UpgradeTypes::Gravitic_Drive);
			if (er == Races::Terran) // to one shot SCVs
				TheProducer->researchUpgrade(UpgradeTypes::Scarab_Damage);
			if (wontHave(UnitTypes::Protoss_Observatory))
				TheBuilder->build(UnitTypes::Protoss_Observatory);
#ifdef __REAVER_FIRST__
			if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Shuttle))
#endif
				TheProducer->produceAlways(5, UnitTypes::Protoss_Observer, 5);
		}
		else if (ut == UnitTypes::Protoss_Shuttle)
		{
			if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver))
				TheProducer->produceAlways(5, UnitTypes::Protoss_Observer, 5);
		}
		else if (ut == UnitTypes::Protoss_Stargate)
		{
			if (EUnitsFilter::Instance().getNumbersType(UnitTypes::Zerg_Mutalisk) > 10)
			{
				TheProducer->produce(6, UnitTypes::Protoss_Corsair, 80, 2);
				TheProducer->produceAlways(4, UnitTypes::Protoss_Corsair, 2);
			}
			if (wontHave(UnitTypes::Protoss_Fleet_Beacon))
				TheBuilder->build(UnitTypes::Protoss_Fleet_Beacon);
			if (wontHave(UnitTypes::Protoss_Arbiter_Tribunal))
				TheBuilder->build(UnitTypes::Protoss_Arbiter_Tribunal);
		}
		else if (ut == UnitTypes::Protoss_Fleet_Beacon)
		{
			TheProducer->produce(4, UnitTypes::Protoss_Carrier, 80);
			TheProducer->produceAlways(4, UnitTypes::Protoss_Carrier, 5);
		}
		else if (ut == UnitTypes::Protoss_Arbiter_Tribunal)
		{
			TheProducer->produce(2, UnitTypes::Protoss_Arbiter, 80);
			TheProducer->produceAlways(1, UnitTypes::Protoss_Arbiter, 5);
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
	// the following is experimental
	if (unit->getPlayer() != Broodwar->self())
	{
		TheArbitrator->onRemoveObject(unit);
		TheWorkerManager->onRemoveUnit(unit);
		TheInformationManager->onUnitDestroy(unit);
		TheInformationManager->onUnitDiscover(unit);
	}
}

void Macro::expand()
{
	if (TheBasesManager->expand())
		Macro::Instance().expands += 1;
}
