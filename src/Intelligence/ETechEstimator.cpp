#include <PrecompiledHeader.h>
#include "Intelligence/ETechEstimator.h"	
#include "enums_name_tables_tt.h"

#define LEARNED_TIME_LIMIT 1080 // 18 minutes

using namespace BWAPI;
using namespace std;

ETechEstimator::ETechEstimator()
: notFirstOverlord(false)
{
	/// Load the learned prob tables (uniforms + bell shapes) for the right match up
	{
		// TODO Random case!!
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
		string serializedTablesFileName("C:\\StarCraft\\AI\\BroodwarBotQ\\data\\tables\\");
		if (enemyRace == Races::Terran)
			serializedTablesFileName.append("TvPx.table");
		else if (enemyRace == Races::Protoss)
			serializedTablesFileName.append("PvPx.table");
		else if (enemyRace == Races::Zerg)
			serializedTablesFileName.append("ZvPx.table");
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
	if (u->getPlayer()->isEnemy(Broodwar->self()))
	{
		int recomputeTime = 0;
		if (u->getType().isBuilding() 
			|| u->getType() == UnitTypes::Zerg_Overlord)
		{
			if (insertBuilding(u))
				recomputeTime = Broodwar->getFrameCount() / 24;
		} else {
			/// We infer the buildings needed to produce this unit
			for (map<UnitType, int>::const_iterator it = u->getType().requiredUnits().begin();
				it != u->getType().requiredUnits().end(); ++it)
			{
				if (insertBuilding(it->first))
				{
					int tmpTime = (Broodwar->getFrameCount()
						- (it->first.buildTime()
						// + walktime
						)) / 24;
					if (!recomputeTime || tmpTime < recomputeTime) // not quite correct
						recomputeTime = tmpTime;
				}
			}
		}
		if (recomputeTime)
			computeDistribOpenings(recomputeTime);
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
		long double sumX = 0.0;
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
		if (openingsProbas[i] == openingsProbas[i] // test for NaN / 1#IND
		    || openingsProbas[i] < 0.0001)         // min proba
			openingsProbas[i] = 0.0001;
		verifSum += openingsProbas[i];
	}
	if (verifSum < 0.99 || verifSum > 1.01)
	{
		for (size_t i = 0; i < openingsProbas.size(); ++i)
			openingsProbas[i] /= verifSum;
	}
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
