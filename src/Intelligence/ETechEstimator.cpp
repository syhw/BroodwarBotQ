#include <PrecompiledHeader.h>
#include "Intelligence/ETechEstimator.h"	
#include "enums_name_tables_tt.h"

using namespace BWAPI;
using namespace std;

ETechEstimator::ETechEstimator()
{
	{
		std::ifstream ifs("C:\\StarCraft\\AI\\BroodwarBotQ\\data\\tables\\PvP.table");
		boost::archive::text_iarchive ia(ifs);
		ia >> st;
	}
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
		if (u->getType().isBuilding() 
			|| u->getType() == UnitTypes::Zerg_Overlord)
		{
			insertBuilding(u);
		}
	}

}

void ETechEstimator::onUnitHide(Unit* u)
{
}

void ETechEstimator::insertBuilding(Unit* u)
{
	UnitType ut = u->getType();
	if (ut.getRace() == BWAPI::Races::Protoss)
	{
		if (ut == UnitTypes::Protoss_Nexus)
		{
			int tmp = Protoss_Nexus;
			while (buildingsSeen.count(u))
				++tmp;
			if (tmp <= Protoss_Nexus3)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Protoss_Robotics_Facility)
			buildingsTypesSeen.insert(Protoss_Robotics_Facility);
		else if (ut == UnitTypes::Protoss_Pylon)
		{
			int tmp = Protoss_Pylon;
			while (buildingsSeen.count(u))
				++tmp;
			if (tmp <= Protoss_Pylon3)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Protoss_Robotics_Facility)
			buildingsTypesSeen.insert(Protoss_Robotics_Facility);
		else if (ut == UnitTypes::Protoss_Assimilator)
		{
			int tmp = Protoss_Assimilator;
			while (buildingsSeen.count(u))
				++tmp;
			if (tmp <= Protoss_Assimilator2)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Protoss_Observatory)
			buildingsTypesSeen.insert(Protoss_Observatory);
		else if (ut == UnitTypes::Protoss_Gateway)
		{
			int tmp = Protoss_Gateway;
			while (buildingsSeen.count(u))
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
			while (buildingsSeen.count(u))
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
			while (buildingsSeen.count(u))
				++tmp;
			if (tmp <= Terran_Supply_Depot3)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Terran_Refinery)
		{
			int tmp = Terran_Refinery;
			while (buildingsSeen.count(u))
				++tmp;
			if (tmp <= Terran_Refinery2)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Terran_Barracks)
		{
			int tmp = Terran_Barracks;
			while (buildingsSeen.count(u))
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
			while (buildingsSeen.count(u))
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
			while (buildingsSeen.count(u))
				++tmp;
			if (tmp <= Zerg_Extractor2)
				buildingsTypesSeen.insert(tmp);
		}
		else if (ut == UnitTypes::Zerg_Overlord)
		{
			int tmp = Zerg_Overlord;
			while (buildingsSeen.count(u))
				++tmp;
			if (tmp <= Zerg_Overlord3)
				buildingsTypesSeen.insert(tmp);
		}
	}
	else
	{
		BWAPI::Broodwar->printf("ERROR: ETechEstimator failed to determine the race of %s", ut.getName().c_str());
	}
	buildingsSeen.insert(u);
}