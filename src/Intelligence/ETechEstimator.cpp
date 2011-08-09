#include <PrecompiledHeader.h>
#include "Intelligence/ETechEstimator.h"	
#include "enums_name_tables_tt.h"
#include "Macro/Producer.h"
#include "Macro/Builder.h"
#include "Intelligence/Intelligence.h"
#include "Macro/Macro.h"
#include "Utils/Util.h"

#define LEARNED_TIME_LIMIT 1080 // 18 minutes

using namespace BWAPI;
using namespace std;

ETechEstimator::ETechEstimator()
: notFirstOverlord(false)
, hasInfered(false)
{
	/// Load the learned prob tables (uniforms + bell shapes) for the right match up
	/// This is not obfuscation, the learning of these tables is described in the CIG 2011 Paper
	/// A Bayesian Model for Opening Prediction in RTS Games with Application to StarCraft, Gabriel Synnaeve, Pierre Bessière, CIG (IEEE) 2011
	/// All code for the learning is here: https://github.com/SnippyHolloW/OpeningTech
	{
		// TODO Random case (load all, use the good one)
		// do __NOT__ use Intelligence::Instance().enemyRace other wise infinite constructors loop and stack overflow and carnage ensues
		Race enemyRace;
		for (set<Player*>::const_iterator p = Broodwar->getPlayers().begin();
			p != Broodwar->getPlayers().end(); ++p)
		{
			if (!(*p)->isNeutral() && (*p)->isEnemy(Broodwar->self()))
			{
				enemyRace = (*p)->getRace();
				break;
			}
		}
		string serializedTablesFileName("bwapi-data/AI/tables/");
		//string serializedTablesFileName("C:\\StarCraft\\AI\\BroodwarBotQ\\data\\tables\\");
		if (enemyRace == Races::Terran || !enemyRace.getName().compare("Terran"))
#ifdef __BENS_LABELS__
			serializedTablesFileName.append("TvP.table");
#else
			serializedTablesFileName.append("TvPx.table");
#endif
		else if (enemyRace == Races::Protoss || !enemyRace.getName().compare("Protoss"))
#ifdef __BENS_LABELS__
			serializedTablesFileName.append("PvP.table");
#else
			serializedTablesFileName.append("PvPx.table");
#endif
		else if (enemyRace == Races::Zerg || !enemyRace.getName().compare("Zerg"))
#ifdef __BENS_LABELS__
			serializedTablesFileName.append("ZvP.table");
#else
			serializedTablesFileName.append("ZvPx.table");
#endif
		std::ifstream ifs(serializedTablesFileName.c_str());
		boost::archive::text_iarchive ia(ifs);
		ia >> st;
	}

	/// Initialize openingsProbas with a uniform distribution
	size_t nbOpenings = st.openings.size();
	for (size_t i = 0; i < nbOpenings; ++i)
		openingsProbas.push_back(1.0 / nbOpenings); 
}

ETechEstimator::~ETechEstimator()
{
}

void ETechEstimator::onUnitDestroy(Unit* u)
{
}

void ETechEstimator::onUnitShow(Unit* u)
{
	if (Broodwar->getFrameCount()/24 >= LEARNED_TIME_LIMIT)
		return;

	if (u->getPlayer()->isEnemy(Broodwar->self())
		&& !alreadySaw(u->getType()))
	{
		int recomputeTime = 0;
		if (u->getType().isBuilding() 
			|| u->getType() == UnitTypes::Zerg_Overlord)
		{
			/// We are interested in the time at which the construction began
			if (insertBuilding(u))
			{
				if (u->isCompleted())
					recomputeTime = (Broodwar->getFrameCount() - u->getType().buildTime()) / 24;
				else
					recomputeTime = (Broodwar->getFrameCount() - u->getRemainingBuildTime()) / 24;
			}
		} else {
			/// We infer the buildings needed to produce this unit
			for (map<UnitType, int>::const_iterator it = u->getType().requiredUnits().begin();
				it != u->getType().requiredUnits().end(); ++it)
			{
				if (insertBuilding(it->first))
				{
					/// The later he could have built this building
					int tmpTime = (Broodwar->getFrameCount()
						- (it->first.buildTime()
						+ u->getType().buildTime() // minimum build time
						//+ u->getDistance(enemyStart, u->getPosition()) / u->getType().topSpeed() // TODO minimum walktime 
						)) / 24;
					if (!recomputeTime || tmpTime > recomputeTime) // we do only one recompute (the final) instead of many, for each buildings
						recomputeTime = tmpTime;
				}
			}
		}
		if (recomputeTime)
		{
			computeDistribOpenings(recomputeTime);
			useDistribOpenings();
		}
	}
}

void ETechEstimator::onUnitHide(Unit* u)
{
}

#ifdef __DEBUG__
void ETechEstimator::onFrame()
{
	Broodwar->drawTextScreen(510, 150, "Opening Prediction");
	Broodwar->drawLineScreen(490, 148, 630, 148, BWAPI::Colors::Blue);
	Broodwar->drawLineScreen(490, 148, 490, 170 + 18*openingsProbas.size(), BWAPI::Colors::Blue);
	Broodwar->drawLineScreen(630, 148, 630, 170 + 18*openingsProbas.size(), BWAPI::Colors::Blue);
	Broodwar->drawLineScreen(490, 170 + 18*openingsProbas.size(), 630, 170 + 18*openingsProbas.size(), BWAPI::Colors::Blue);
	for (size_t i = 0; i < openingsProbas.size(); ++i)
		Broodwar->drawTextScreen(500, 170+18*i, "%s ==> %Lg", st.openings[i].c_str(), openingsProbas[i]);
}
#endif

bool ETechEstimator::alreadySaw(UnitType ut)
{
	bool ret = (_alreadySawUnitTypes.find(ut) != _alreadySawUnitTypes.end());
	if (!ret)
		_alreadySawUnitTypes.insert(ut);
	return ret;
}

bool ETechEstimator::insertBuilding(UnitType ut)
{
	size_t previous_size = buildingsTypesSeen.size();
	if (ut.getRace() == BWAPI::Races::Protoss)
	{
		if (ut == UnitTypes::Protoss_Robotics_Facility)
			buildingsTypesSeen.insert(Protoss_Robotics_Facility);
		else if (ut == UnitTypes::Protoss_Observatory)
			buildingsTypesSeen.insert(Protoss_Observatory);
		else if (ut == UnitTypes::Protoss_Gateway)
			buildingsTypesSeen.insert(Protoss_Gateway);
		else if (ut == UnitTypes::Protoss_Citadel_of_Adun)
			buildingsTypesSeen.insert(Protoss_Citadel_of_Adun);
		else if (ut == UnitTypes::Protoss_Cybernetics_Core)
			buildingsTypesSeen.insert(Protoss_Cybernetics_Core);
		else if (ut == UnitTypes::Protoss_Templar_Archives)
			buildingsTypesSeen.insert(Protoss_Templar_Archives);
		else if (ut == UnitTypes::Protoss_Forge)
			buildingsTypesSeen.insert(Protoss_Forge);
		else if (ut == UnitTypes::Protoss_Stargate)
			buildingsTypesSeen.insert(Protoss_Stargate);
		else if (ut == UnitTypes::Protoss_Fleet_Beacon)
			buildingsTypesSeen.insert(Protoss_Fleet_Beacon);
		else if (ut == UnitTypes::Protoss_Arbiter_Tribunal)
			buildingsTypesSeen.insert(Protoss_Arbiter_Tribunal);
		else if (ut == UnitTypes::Protoss_Robotics_Support_Bay)
			buildingsTypesSeen.insert(Protoss_Robotics_Support_Bay);
	}
	else if (ut.getRace() == BWAPI::Races::Terran)
	{
		if (ut == UnitTypes::Terran_Comsat_Station)
			buildingsTypesSeen.insert(Terran_ComSat);
		else if (ut == UnitTypes::Terran_Nuclear_Silo)
			buildingsTypesSeen.insert(Terran_Nuclear_Silo);
		else if (ut == UnitTypes::Terran_Barracks)
			buildingsTypesSeen.insert(Terran_Barracks);
		else if (ut == UnitTypes::Terran_Academy)
			buildingsTypesSeen.insert(Terran_Academy);
		else if (ut == UnitTypes::Terran_Factory)
			buildingsTypesSeen.insert(Terran_Factory);
		else if (ut == UnitTypes::Terran_Starport)
			buildingsTypesSeen.insert(Terran_Starport);
		else if (ut == UnitTypes::Terran_Control_Tower)
			buildingsTypesSeen.insert(Terran_Control_Tower);
		else if (ut == UnitTypes::Terran_Science_Facility)
			buildingsTypesSeen.insert(Terran_Science_Facility);
		else if (ut == UnitTypes::Terran_Covert_Ops)
			buildingsTypesSeen.insert(Terran_Covert_Ops);
		else if (ut == UnitTypes::Terran_Physics_Lab)
			buildingsTypesSeen.insert(Terran_Physics_Lab);
		else if (ut == UnitTypes::Terran_Machine_Shop)
			buildingsTypesSeen.insert(Terran_Machine_Shop);
		else if (ut == UnitTypes::Terran_Engineering_Bay)
			buildingsTypesSeen.insert(Terran_Engineering_Bay);
		else if (ut == UnitTypes::Terran_Armory)
			buildingsTypesSeen.insert(Terran_Armory);
		else if (ut == UnitTypes::Terran_Missile_Turret)
			buildingsTypesSeen.insert(Terran_Missile_Turret);
	}
	else if (ut.getRace() == BWAPI::Races::Zerg)
	{
		if (ut == UnitTypes::Zerg_Lair)
			buildingsTypesSeen.insert(Zerg_Lair);
		else if (ut == UnitTypes::Zerg_Hive)
			buildingsTypesSeen.insert(Zerg_Hive);
		else if (ut == UnitTypes::Zerg_Nydus_Canal)
			buildingsTypesSeen.insert(Zerg_Nydus_Canal);
		else if (ut == UnitTypes::Zerg_Hydralisk_Den)
			buildingsTypesSeen.insert(Zerg_Hydralisk_Den);
		else if (ut == UnitTypes::Zerg_Defiler_Mound)
			buildingsTypesSeen.insert(Zerg_Defiler_Mound);
		else if (ut == UnitTypes::Zerg_Greater_Spire)
			buildingsTypesSeen.insert(Zerg_Greater_Spire);
		else if (ut == UnitTypes::Zerg_Queens_Nest)
			buildingsTypesSeen.insert(Zerg_Queens_Nest);
		else if (ut == UnitTypes::Zerg_Evolution_Chamber)
			buildingsTypesSeen.insert(Zerg_Evolution_Chamber);
		else if (ut == UnitTypes::Zerg_Ultralisk_Cavern)
			buildingsTypesSeen.insert(Zerg_Ultralisk_Cavern);
		else if (ut == UnitTypes::Zerg_Spire)
			buildingsTypesSeen.insert(Zerg_Spire);
		else if (ut == UnitTypes::Zerg_Spawning_Pool)
			buildingsTypesSeen.insert(Zerg_Spawning_Pool);
		else if (ut == UnitTypes::Zerg_Creep_Colony)
			buildingsTypesSeen.insert(Zerg_Creep_Colony);
	}
	else
	{
		BWAPI::Broodwar->printf("ERROR: ETechEstimator failed to determine the race of %s", ut.getName().c_str());
	}
	if (buildingsTypesSeen.size() > previous_size)
		return true;
	else
		return false;
}

bool ETechEstimator::insertBuilding(Unit* u)
{
	UnitType ut = u->getType();
	size_t previous_size = buildingsTypesSeen.size();
	if (ut.getRace() == BWAPI::Races::Protoss)
	{
		if (ut == UnitTypes::Protoss_Nexus)
		{
			int tmp = Protoss_Nexus;
			while (buildingsTypesSeen.count(tmp))
				++tmp;
			if (tmp <= Protoss_Nexus3)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Protoss_Robotics_Facility)
			buildingsTypesSeen.insert(Protoss_Robotics_Facility);
		else if (ut == UnitTypes::Protoss_Pylon)
		{
			int tmp = Protoss_Pylon;
			while (buildingsTypesSeen.count(tmp))
				++tmp;
			if (tmp <= Protoss_Pylon3)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Protoss_Assimilator)
		{
			int tmp = Protoss_Assimilator;
			while (buildingsTypesSeen.count(tmp))
				++tmp;
			if (tmp <= Protoss_Assimilator2)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Protoss_Observatory)
			buildingsTypesSeen.insert(Protoss_Observatory);
		else if (ut == UnitTypes::Protoss_Gateway)
		{
			int tmp = Protoss_Gateway;
			while (buildingsTypesSeen.count(tmp))
				++tmp;
			if (tmp <= Protoss_Gateway4)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Protoss_Photon_Cannon)
			buildingsTypesSeen.insert(Protoss_Photon_Cannon);
		else if (ut == UnitTypes::Protoss_Citadel_of_Adun)
			buildingsTypesSeen.insert(Protoss_Citadel_of_Adun);
		else if (ut == UnitTypes::Protoss_Cybernetics_Core)
			buildingsTypesSeen.insert(Protoss_Cybernetics_Core);
		else if (ut == UnitTypes::Protoss_Templar_Archives)
			buildingsTypesSeen.insert(Protoss_Templar_Archives);
		else if (ut == UnitTypes::Protoss_Forge)
			buildingsTypesSeen.insert(Protoss_Forge);
		else if (ut == UnitTypes::Protoss_Stargate)
			buildingsTypesSeen.insert(Protoss_Stargate);
		else if (ut == UnitTypes::Protoss_Fleet_Beacon)
			buildingsTypesSeen.insert(Protoss_Fleet_Beacon);
		else if (ut == UnitTypes::Protoss_Arbiter_Tribunal)
			buildingsTypesSeen.insert(Protoss_Arbiter_Tribunal);
		else if (ut == UnitTypes::Protoss_Robotics_Support_Bay)
			buildingsTypesSeen.insert(Protoss_Robotics_Support_Bay);
		else if (ut == UnitTypes::Protoss_Shield_Battery)
			buildingsTypesSeen.insert(Protoss_Shield_Battery);
	}
	else if (ut.getRace() == BWAPI::Races::Terran)
	{
		if (ut == UnitTypes::Terran_Command_Center)
		{
			int tmp = Terran_Command_Center;
			while (buildingsTypesSeen.count(tmp))
				++tmp;
			if (tmp <= Terran_Command_Center3)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Terran_Comsat_Station)
			buildingsTypesSeen.insert(Terran_ComSat);
		else if (ut == UnitTypes::Terran_Nuclear_Silo)
			buildingsTypesSeen.insert(Terran_Nuclear_Silo);
		else if (ut == UnitTypes::Terran_Supply_Depot)
		{
			int tmp = Terran_Supply_Depot;
			while (buildingsTypesSeen.count(tmp))
				++tmp;
			if (tmp <= Terran_Supply_Depot3)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Terran_Refinery)
		{
			int tmp = Terran_Refinery;
			while (buildingsTypesSeen.count(tmp))
				++tmp;
			if (tmp <= Terran_Refinery2)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Terran_Barracks)
		{
			int tmp = Terran_Barracks;
			while (buildingsTypesSeen.count(tmp))
				++tmp;
			if (tmp <= Terran_Barracks4)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Terran_Academy)
			buildingsTypesSeen.insert(Terran_Academy);
		else if (ut == UnitTypes::Terran_Factory)
			buildingsTypesSeen.insert(Terran_Factory);
		else if (ut == UnitTypes::Terran_Starport)
			buildingsTypesSeen.insert(Terran_Starport);
		else if (ut == UnitTypes::Terran_Control_Tower)
			buildingsTypesSeen.insert(Terran_Control_Tower);
		else if (ut == UnitTypes::Terran_Science_Facility)
			buildingsTypesSeen.insert(Terran_Science_Facility);
		else if (ut == UnitTypes::Terran_Covert_Ops)
			buildingsTypesSeen.insert(Terran_Covert_Ops);
		else if (ut == UnitTypes::Terran_Physics_Lab)
			buildingsTypesSeen.insert(Terran_Physics_Lab);
		else if (ut == UnitTypes::Terran_Machine_Shop)
			buildingsTypesSeen.insert(Terran_Machine_Shop);
		else if (ut == UnitTypes::Terran_Engineering_Bay)
			buildingsTypesSeen.insert(Terran_Engineering_Bay);
		else if (ut == UnitTypes::Terran_Armory)
			buildingsTypesSeen.insert(Terran_Armory);
		else if (ut == UnitTypes::Terran_Missile_Turret)
			buildingsTypesSeen.insert(Terran_Missile_Turret);
		else if (ut == UnitTypes::Terran_Bunker)
			buildingsTypesSeen.insert(Terran_Bunker);
	}
	else if (ut.getRace() == BWAPI::Races::Zerg)
	{
		if (ut == UnitTypes::Zerg_Hatchery)
		{
			int tmp = Zerg_Hatchery;
			while (buildingsTypesSeen.count(tmp))
				++tmp;
			if (tmp <= Zerg_Hatchery4)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Zerg_Lair)
			buildingsTypesSeen.insert(Zerg_Lair);
		else if (ut == UnitTypes::Zerg_Hive)
			buildingsTypesSeen.insert(Zerg_Hive);
		else if (ut == UnitTypes::Zerg_Nydus_Canal)
			buildingsTypesSeen.insert(Zerg_Nydus_Canal);
		else if (ut == UnitTypes::Zerg_Hydralisk_Den)
			buildingsTypesSeen.insert(Zerg_Hydralisk_Den);
		else if (ut == UnitTypes::Zerg_Defiler_Mound)
			buildingsTypesSeen.insert(Zerg_Defiler_Mound);
		else if (ut == UnitTypes::Zerg_Greater_Spire)
			buildingsTypesSeen.insert(Zerg_Greater_Spire);
		else if (ut == UnitTypes::Zerg_Queens_Nest)
			buildingsTypesSeen.insert(Zerg_Queens_Nest);
		else if (ut == UnitTypes::Zerg_Evolution_Chamber)
			buildingsTypesSeen.insert(Zerg_Evolution_Chamber);
		else if (ut == UnitTypes::Zerg_Ultralisk_Cavern)
			buildingsTypesSeen.insert(Zerg_Ultralisk_Cavern);
		else if (ut == UnitTypes::Zerg_Spire)
			buildingsTypesSeen.insert(Zerg_Spire);
		else if (ut == UnitTypes::Zerg_Spawning_Pool)
			buildingsTypesSeen.insert(Zerg_Spawning_Pool);
		else if (ut == UnitTypes::Zerg_Creep_Colony)
			buildingsTypesSeen.insert(Zerg_Creep_Colony);
		else if (ut == UnitTypes::Zerg_Spore_Colony)
			buildingsTypesSeen.insert(Zerg_Spore_Colony);
		else if (ut == UnitTypes::Zerg_Sunken_Colony)
			buildingsTypesSeen.insert(Zerg_Sunken_Colony);
		else if (ut == UnitTypes::Zerg_Extractor)
		{
			int tmp = Zerg_Extractor;
			while (buildingsTypesSeen.count(tmp))
				++tmp;
			if (tmp <= Zerg_Extractor2)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Zerg_Overlord)
		{
			if (notFirstOverlord)
			{
				int tmp = Zerg_Overlord;
				while (buildingsTypesSeen.count(tmp))
					++tmp;
				if (tmp <= Zerg_Overlord3)
					buildingsTypesSeen.insert(tmp);
			}
			else
				notFirstOverlord = true;
		}
	}
	else
	{
		BWAPI::Broodwar->printf("ERROR: ETechEstimator failed to determine the race of %s", ut.getName().c_str());
	}
	buildingsSeen.insert(u);
	if (buildingsTypesSeen.size() > previous_size)
		return true;
	else
		return false;
}

void ETechEstimator::computeDistribOpenings(int time)
{
	if (time >= LEARNED_TIME_LIMIT || time <= 0)
		return;

	size_t nbXes = st.vector_X.size();
	list<unsigned int> compatibleXes;
	for (size_t i = 0; i < nbXes; ++i)
	{
		if (testBuildTreePossible(i, buildingsTypesSeen))
			compatibleXes.push_back(i);
	}
	long double runningSum = 0.0;
	for (size_t i = 0; i < openingsProbas.size(); ++i)
	{
		long double sumX = 0.00000000000000000001;
		for (list<unsigned int>::const_iterator it = compatibleXes.begin();
			it != compatibleXes.end(); ++it)
		{
			sumX += st.tabulated_P_X_Op[(*it) * openingsProbas.size() + i]
			* st.tabulated_P_Time_X_Op[(*it) * openingsProbas.size() * LEARNED_TIME_LIMIT
				+ i * LEARNED_TIME_LIMIT + time];
		}
		openingsProbas[i] *= sumX;
		runningSum += openingsProbas[i];
	}
	long double verifSum = 0.0;
	for (size_t i = 0; i < openingsProbas.size(); ++i)
	{
		openingsProbas[i] /= runningSum;
		if (openingsProbas[i] != openingsProbas[i] // test for NaN / 1#IND
		//|| openingsProbas[i] < 0.00000000000000000001         // min proba
		)
			openingsProbas[i] = 0.00000000000000000001;
		verifSum += openingsProbas[i];
	}
	if (verifSum < 0.99 || verifSum > 1.01)
	{
		for (size_t i = 0; i < openingsProbas.size(); ++i)
			openingsProbas[i] /= verifSum;
	}
	hasInfered = true;
}

/**
* Tests if the given build tree (X) value
* is compatible with obervations (what have been seen)
* {X ^ observed} covers all observed if X is possible
* so X is impossible if {observed \ {X ^ observed}} != {}
* => X is compatible with observations if it covers them fully
*/
bool ETechEstimator::testBuildTreePossible(int indBuildTree, const set<int>& setObs)
{
	for (set<int>::const_iterator it = setObs.begin();
		it != setObs.end(); ++it)
	{
		if (!st.vector_X[indBuildTree].count(*it))
			return false;
	}
	return true;
}

/// TODO: move all that in appropriate methods for specific
/// "reactions to some opening/strat/tactic" in the relevant managers
/// (Bullshit talk (c))
void ETechEstimator::useDistribOpenings()
{
	if (Broodwar->getFrameCount()/24 >= LEARNED_TIME_LIMIT)
		return;
	/// here we should now for sure the enemy's race as we have seen at least a building;
	Race enemyRace = Intelligence::Instance().enemyRace;

	size_t mostProbable = indMax(openingsProbas);
	set<size_t> fearThese = supTo(openingsProbas, 0.20);
#ifdef __BENS_LABELS__
	int builtCannons = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Photon_Cannon)
		+ TheBuilder->willBuild(UnitTypes::Protoss_Photon_Cannon); // ~~
	if (enemyRace == Races::Terran)
	{
		if (fearThese.count(0)) // Bio
		{
			if (openingsProbas[0] > openingsProbas[1] && openingsProbas[0] > openingsProbas[2])
				Macro::Instance().stormFirst = true;
		}

		if (fearThese.count(2)) // VulturesHarass
		{
			TheProducer->produce(2, UnitTypes::Protoss_Observer, (int)(openingsProbas[2]*100));
#ifdef __DEBUG__
			Broodwar->printf("Producing observers bc of Vultures");
#endif
		}

		if (fearThese.count(3)) // SiegeExpand
		{
			if (!Macro::Instance().expands)
				Macro::Instance().expand();
		}

		if (fearThese.count(5)) // FastDropship
		{
			while (builtCannons < 3 + 1*Macro::Instance().expands) 
			{
				TheBuilder->build(UnitTypes::Protoss_Photon_Cannon);
				++builtCannons;
			}
#ifdef __DEBUG__
			Broodwar->printf("Building cannons bc of Dropship");
#endif
		}
	}
	else if (enemyRace == Races::Protoss)
	{
		if (fearThese.empty() && Broodwar->getFrameCount() > 1440)
		{
			TheProducer->produce(2, UnitTypes::Protoss_Zealot, 60, 2);
		}
		if (fearThese.count(1) || openingsProbas[1] > 0.15) // FastDT
		{
			if (!Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Forge)
				&& !TheBuilder->willBuild(UnitTypes::Protoss_Forge))
			{
				if (Broodwar->self()->supplyUsed() < 42)
					TheBuilder->buildOrder(UnitTypes::Protoss_Forge, 16);
				else
					TheBuilder->build(UnitTypes::Protoss_Forge, TilePositions::None, true); // quickly build a forge
			}
			if ((Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Forge)
				|| Broodwar->self()->incompleteUnitCount(UnitTypes::Protoss_Forge))
				&& builtCannons < 3)
			{
				if (Broodwar->self()->supplyUsed() < 42)
				{
					TheBuilder->build(UnitTypes::Protoss_Photon_Cannon, TilePositions::None, true);
					TheBuilder->build(UnitTypes::Protoss_Photon_Cannon, TilePositions::None, true);
				}
				else
				{
					TheBuilder->buildOrder(UnitTypes::Protoss_Photon_Cannon, 19);
					TheBuilder->buildOrder(UnitTypes::Protoss_Photon_Cannon, 19);
				}
				builtCannons += 2;
			}
			while (builtCannons < 3 + 2*Macro::Instance().expands) 
			{
				TheBuilder->build(UnitTypes::Protoss_Photon_Cannon);
				++builtCannons;
			}
			Macro::Instance().stormFirst = false;
			Macro::Instance().reaverFirst = false; // will do Obs first then
#ifdef __DEBUG__
			Broodwar->printf("Building cannons bc of DTs");
			Broodwar->printf("Producing observers bc of DTs");
#endif
		}

		if (fearThese.count(3)) // ReaverDrop
		{
			while (builtCannons < 3 + 1*Macro::Instance().expands) 
			{
				TheBuilder->build(UnitTypes::Protoss_Photon_Cannon);
				++builtCannons;
			}
#ifdef __DEBUG__
			Broodwar->printf("Building cannons bc of ReaverDrop");
#endif
		}

		if (fearThese.count(5) // FastExpand
			&& openingsProbas[5] > 0.35)
		{
			if (!Macro::Instance().expands)
				Macro::Instance().expand();
		}
	}
	else if (enemyRace == Races::Zerg)
	{
		if (fearThese.count(0)) // TwoHatchMuta
		{
			while (builtCannons < 3 + 1*Macro::Instance().expands) 
			{
				TheBuilder->build(UnitTypes::Protoss_Photon_Cannon);
				++builtCannons;
			}
#ifdef __DEBUG__
			Broodwar->printf("Building cannons bc of 2H Mutas");
#endif
		}
		
		if (fearThese.count(1)) // ThreeHatchMuta
		{
			while (builtCannons < 3 + 1*Macro::Instance().expands) 
			{
				TheBuilder->build(UnitTypes::Protoss_Photon_Cannon);
				++builtCannons;
			}
#ifdef __DEBUG__
			Broodwar->printf("Building cannons bc of 3H Mutas");
#endif
		}

		if (openingsProbas[0] > openingsProbas[5] && openingsProbas[1] > openingsProbas[5]) // storm against mutas, if Mutas > Lurkers
			Macro::Instance().stormFirst = true;

		if (fearThese.count(5)) // Lurker
		{
			Macro::Instance().reaverFirst = false; // we will need obs
			if (!Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility)
				&& !TheBuilder->willBuild(UnitTypes::Protoss_Robotics_Facility))
			{
				TheBuilder->build(UnitTypes::Protoss_Robotics_Facility, TilePositions::None, true);
			}
			TheProducer->produce(3, UnitTypes::Protoss_Observer, max((int)(openingsProbas[1]*100*2), 95));
#ifdef __DEBUG__
			Broodwar->printf("Producing observers bc of Lurkers");
#endif
		}
	}
#endif
}

const std::vector<long double>& ETechEstimator::getOpeningsProbas() const
{
	return openingsProbas;
}