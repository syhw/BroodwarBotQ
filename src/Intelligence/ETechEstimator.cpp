#include <PrecompiledHeader.h>
#include "Intelligence/ETechEstimator.h"	
#include "enums_name_tables_tt.h"
#include "Macro/Producer.h"
#include "Macro/Builder.h"
#include "Intelligence/Intelligence.h"
#include "Macro/Macro.h"
#include "Utils/Util.h"

#define LEARNED_TIME_LIMIT 1080 // 18 minutes
#define MIN_PROB 0.00000000000000000001

using namespace BWAPI;
using namespace std;

int format_max_time(int t)
{
	return min((LEARNED_TIME_LIMIT-1) / 60, t / 60);
}

void ETechEstimator::loadTable(const char* tname)
{
	ifstream ifs(tname);
	boost::archive::text_iarchive ia(ifs);
	ia >> st;
	tableLoaded = true;
}

ETechEstimator::ETechEstimator()
: notFirstOverlord(false)
, hasInfered(false)
, tableLoaded(false)
{
	/// Load the learned prob tables (uniforms + bell shapes) for the right match up
	/// This is not obfuscation, the learning of these tables is described in the CIG 2011 Paper
	/// A Bayesian Model for Opening Prediction in RTS Games with Application to StarCraft, Gabriel Synnaeve, Pierre Bessière, CIG (IEEE) 2011
	/// All code for the learning is here: https://github.com/SnippyHolloW/OpeningTech
	{
		// do __NOT__ use Intelligence::Instance().enemyRace otherwise infinite constructors loop and stack overflow and carnage ensues
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
		{
#ifdef __BENS_LABELS__
			serializedTablesFileName.append("TvP.table");
#else
			serializedTablesFileName.append("TvPx.table");
#endif
			loadTable(serializedTablesFileName.c_str());
		}
		else if (enemyRace == Races::Protoss || !enemyRace.getName().compare("Protoss"))
		{
#ifdef __BENS_LABELS__
			serializedTablesFileName.append("PvP.table");
#else
			serializedTablesFileName.append("PvPx.table");
#endif
			loadTable(serializedTablesFileName.c_str());
		}
		else if (enemyRace == Races::Zerg || !enemyRace.getName().compare("Zerg"))
		{
#ifdef __BENS_LABELS__
			serializedTablesFileName.append("ZvP.table");
#else
			serializedTablesFileName.append("ZvPx.table");
#endif
			loadTable(serializedTablesFileName.c_str());
		}
		else // RANDOM
		{
			string tmpTvP(serializedTablesFileName);
			string tmpZvP(serializedTablesFileName);
#ifdef __BENS_LABELS__
			serializedTablesFileName.append("PvP.table");
			tmpTvP.append("TvP.table");
			tmpZvP.append("ZvP.table");
#else
			serializedTablesFileName.append("PvPx.table");
			tmpTvP.append("TvPx.table");
			tmpZvP.append("ZvPx.table");
#endif
			ifstream ifs(serializedTablesFileName.c_str());
			boost::archive::text_iarchive ia(ifs);
			ia >> st;
			ifstream ifs2(tmpTvP.c_str());
			boost::archive::text_iarchive ia2(ifs2);
			ia2 >> TvP;
			ifstream ifs3(tmpZvP.c_str());
			boost::archive::text_iarchive ia3(ifs3);
			ia3 >> ZvP;
		}
	}

	/// Initialize openingsProbas with a uniform distribution
	size_t nbOpenings = st.openings.size();
	openingsProbas = vector<long double>(nbOpenings, 1.0 / nbOpenings);

	/// Initialize op_priors from what we saw of the opponent
	string filename("bwapi-data/read/op_prior_");
	filename.append(Broodwar->enemy()->getName());
	filename.append("_");
	filename.append(Broodwar->enemy()->getRace().c_str());
	ifstream op_priors(filename.c_str());
	if (op_priors.good())
	{
		boost::archive::text_iarchive op_priors_archive(op_priors);
		op_priors_archive >> op_prior;
	}
	else
	{
		// we never played against this opponent => uniform prior
		op_prior.numberGames = vector<int>(LEARNED_TIME_LIMIT / 60, 1);
		for (size_t t = 0; t < LEARNED_TIME_LIMIT / 60; ++t)
			op_prior.tabulated_P_Op_knowing_Time
				.push_back(vector<long double>(nbOpenings, 1.0 / nbOpenings));
	}
}

ETechEstimator::~ETechEstimator()
{
	/// Write op_priors from what we saw of the opponent
	{
		string filename("bwapi-data/read/op_prior_"); // TODO CHANGE IN WRITE FOLDER
		filename.append(Broodwar->enemy()->getName());
		filename.append("_");
		filename.append(Broodwar->enemy()->getRace().c_str());
		ofstream op_priors(filename.c_str());
		if (op_priors.good())
		{
			boost::archive::text_oarchive op_priors_archive(op_priors);
			op_priors_archive << op_prior;
		}
	}
}

void ETechEstimator::onUnitDestroy(Unit* u)
{
}

void ETechEstimator::onUnitShow(Unit* u)
{
	if (Broodwar->getFrameCount()/24 >= LEARNED_TIME_LIMIT
		|| u->getPlayer()->isNeutral())
		return;

	if (u->getPlayer()->isEnemy(Broodwar->self())
		&& !alreadySaw(u->getType()))
	{
		/// If it's the first enemy unit that we see and he is random,
		/// update st with the good serialized_table
		if (!tableLoaded)
		{
			if (u->getType().getRace() == Races::Terran)
				st.swap(TvP);
			else if (u->getType().getRace() == Races::Zerg)
				st.swap(ZvP);
			tableLoaded = true;
		}

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
						//+ u->getDistance(enemyStart, u->getPosition()) / u->getType().topSpeed() // minimum walking distance done next line (approx.)
						+ static_cast<int>(((Broodwar->mapWidth() + Broodwar->mapHeight())/2.0 * TILE_SIZE) / u->getType().topSpeed())
						)) / 24;
					if (!recomputeTime || tmpTime > recomputeTime) // we do only one recompute (the final) instead of many, for each buildings
						recomputeTime = tmpTime;
				}
			}
		}
		if (recomputeTime)
		{
			computeDistribOpenings(recomputeTime);
			useDistribOpenings(__ETECHESTIMATOR_MINUTES__*60);
		}
	}
}

void ETechEstimator::onUnitHide(Unit* u)
{
}

#ifdef __ETECHESTIMATOR_DEBUG__
void ETechEstimator::onFrame()
{
	vector<long double> tmpOpProb(openingsProbas);
#if __ETECHESTIMATOR_MINUTES__ > 0
	tmpOpProb = getOpeningsProbasIn(__ETECHESTIMATOR_MINUTES__);
	Broodwar->drawTextScreen(480, 150, "Strategy Prediction t+%d (percent)", __ETECHESTIMATOR_MINUTES__);
#else
	Broodwar->drawTextScreen(510, 150, "Strategy Estimation (percent)");
#endif
	if (!tableLoaded)
		return;
	Broodwar->drawLineScreen(490, 148, 630, 148, BWAPI::Colors::Blue);
	Broodwar->drawLineScreen(490, 148, 490, 170 + 18*tmpOpProb.size(), BWAPI::Colors::Blue);
	Broodwar->drawLineScreen(630, 148, 630, 170 + 18*tmpOpProb.size(), BWAPI::Colors::Blue);
	Broodwar->drawLineScreen(490, 170 + 18*tmpOpProb.size(), 630, 170 + 18*tmpOpProb.size(), BWAPI::Colors::Blue);
	for (size_t i = 0; i < tmpOpProb.size(); ++i)
		Broodwar->drawTextScreen(500, 170+18*i, "%s ==> %Lg", st.openings[i].c_str(), tmpOpProb[i]*100);
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
	if (ut.getRace() == Races::Protoss)
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
	else if (ut.getRace() == Races::Terran)
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
	else if (ut.getRace() == Races::Zerg)
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
		Broodwar->printf("ERROR: ETechEstimator failed to determine the race of %s", ut.getName().c_str());
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
	if (ut.getRace() == Races::Protoss)
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
	else if (ut.getRace() == Races::Terran)
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
	else if (ut.getRace() == Races::Zerg)
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
		Broodwar->printf("ERROR: ETechEstimator failed to determine the race of %s", ut.getName().c_str());
	}
	buildingsSeen.insert(u);
	if (buildingsTypesSeen.size() > previous_size)
		return true;
	else
		return false;
}

vector<long double> ETechEstimator::computeVecDistribOpenings(int time)
{
	vector<long double> ret(openingsProbas.size(), 1.0/openingsProbas.size());
	/// time in seconds
	if (time >= LEARNED_TIME_LIMIT || time <= 0)
		return ret; // we don't know and thus give back the uniform distrib

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
		long double sumX = MIN_PROB;
		for (list<unsigned int>::const_iterator it = compatibleXes.begin();
			it != compatibleXes.end(); ++it)
		{ /// perhaps underflow? log-prob?
			sumX += op_prior.tabulated_P_Op_knowing_Time
				[format_max_time(time)][i]
			* st.tabulated_P_X_Op[(*it) * openingsProbas.size() + i]
			* st.tabulated_P_Time_X_Op[(*it) * openingsProbas.size() * LEARNED_TIME_LIMIT
				+ i * LEARNED_TIME_LIMIT + time];
		}
		ret[i] *= sumX;
		runningSum += ret[i];
	}
	long double verifSum = 0.0;
	for (size_t i = 0; i < openingsProbas.size(); ++i)
	{
		ret[i] /= runningSum;
		if (ret[i] != ret[i] // test for NaN / 1#IND
		//|| openingsProbas[i] < MIN_PROB        // min proba
		)
			ret[i] = MIN_PROB;
		verifSum += ret[i];
	}
	if (verifSum < 0.99 || verifSum > 1.01)
	{
		for (size_t i = 0; i < openingsProbas.size(); ++i)
			ret[i] /= verifSum;
	}
	return ret;
}

void ETechEstimator::computeDistribOpenings(int time)
{
	/// time in seconds
	if (time >= LEARNED_TIME_LIMIT || time <= 0)
		return;
	openingsProbas.swap(computeVecDistribOpenings(time));
	for (size_t i = 0; i < openingsProbas.size(); ++i)
	{
		int t = format_max_time(time);
		// cumulative moving average: ca_{n+1} = (x_{n+1}+n*ca_{n})/(n+1)
		op_prior.tabulated_P_Op_knowing_Time[t][i]
		= (openingsProbas[i] + op_prior.numberGames[t] 
		  *op_prior.tabulated_P_Op_knowing_Time[t][i])
			  / (op_prior.numberGames[t] + 1);
		op_prior.numberGames[t] += 1;
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
void ETechEstimator::useDistribOpenings(int time)
{
	if (time + Broodwar->getFrameCount()/24 >= LEARNED_TIME_LIMIT)
		return;
	/// here we should now for sure the enemy's race as we have seen at least a building;
	Race enemyRace = Intelligence::Instance().enemyRace;

	vector<long double> tmpOpProb(openingsProbas); 
	if (time > 0)
		tmpOpProb = getOpeningsProbasIn(time);
	
	size_t mostProbable = indMax(tmpOpProb);
	set<size_t> fearThese = supTo(tmpOpProb, 0.20);
#ifdef __BENS_LABELS__
	int builtCannons = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Photon_Cannon)
		+ TheBuilder->willBuild(UnitTypes::Protoss_Photon_Cannon); // ~~
	if (enemyRace == Races::Terran)
	{
		if (fearThese.count(2)) // VulturesHarass
		{
			TheProducer->produce(2, UnitTypes::Protoss_Observer, (int)(tmpOpProb[2]*100));
#ifdef __INTELLIGENCE_DEBUG__
			Broodwar->printf("Producing observers bc of Vultures");
#endif
		}
		else if (fearThese.count(3)) // SiegeExpand
		{
			if (!Macro::Instance().expands && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 4)
				Macro::Instance().expand();
		}
		else if (fearThese.count(0)) // Bio
		{
			if (tmpOpProb[0] > tmpOpProb[1] && tmpOpProb[0] > tmpOpProb[2])
				Macro::Instance().stormFirst = true;
		}
		else if (fearThese.count(5)) // FastDropship
		{
			while (builtCannons < 3 + 1*Macro::Instance().expands) 
			{
				TheBuilder->build(UnitTypes::Protoss_Photon_Cannon);
				++builtCannons;
			}
#ifdef __INTELLIGENCE_DEBUG__
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
		else if (fearThese.count(1) || tmpOpProb[1] > 0.15) // FastDT
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
#ifdef __INTELLIGENCE_DEBUG__
			Broodwar->printf("Building cannons bc of DTs");
			Broodwar->printf("Producing observers bc of DTs");
#endif
		}
		else if (fearThese.count(3)) // ReaverDrop
		{
			while (builtCannons < 3 + 1*Macro::Instance().expands) 
			{
				TheBuilder->build(UnitTypes::Protoss_Photon_Cannon);
				++builtCannons;
			}
#ifdef __INTELLIGENCE_DEBUG__
			Broodwar->printf("Building cannons bc of ReaverDrop");
#endif
		}
		else if (fearThese.count(5) // FastExpand
			&& tmpOpProb[5] > 0.35)
		{
			if (!Macro::Instance().expands && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 3)
				Macro::Instance().expand();
		}
	}
	else if (enemyRace == Races::Zerg)
	{
		if (fearThese.count(5)) // Lurker
		{
			Macro::Instance().reaverFirst = false; // we will need obs
			if (!Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility)
				&& !TheBuilder->willBuild(UnitTypes::Protoss_Robotics_Facility))
			{
				TheBuilder->build(UnitTypes::Protoss_Robotics_Facility, TilePositions::None, true);
			}
			TheProducer->produce(3, UnitTypes::Protoss_Observer, max((int)(tmpOpProb[1]*100*2), 95));
#ifdef __INTELLIGENCE_DEBUG__
			Broodwar->printf("Producing observers bc of Lurkers");
#endif
		}
		else if (fearThese.count(0)) // TwoHatchMuta
		{
			while (builtCannons < 3 + 1*Macro::Instance().expands) 
			{
				TheBuilder->build(UnitTypes::Protoss_Photon_Cannon);
				++builtCannons;
			}
#ifdef __INTELLIGENCE_DEBUG__
			Broodwar->printf("Building cannons bc of 2H Mutas");
#endif
		}
		else if (fearThese.count(1)) // ThreeHatchMuta
		{
			while (builtCannons < 3 + 1*Macro::Instance().expands) 
			{
				TheBuilder->build(UnitTypes::Protoss_Photon_Cannon);
				++builtCannons;
			}
#ifdef __INTELLIGENCE_DEBUG__
			Broodwar->printf("Building cannons bc of 3H Mutas");
#endif
		}

		if (tmpOpProb[0] > tmpOpProb[5] && tmpOpProb[1] > tmpOpProb[5]) // storm against mutas, if Mutas > Lurkers
			Macro::Instance().stormFirst = true;
	}
#endif
}

const vector<long double>& ETechEstimator::getOpeningsProbas() const
{
	return openingsProbas;
}

vector<long double> ETechEstimator::getOpeningsProbasIn(int time)
{
	return computeVecDistribOpenings(time);
}