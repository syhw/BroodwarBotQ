#include <PrecompiledHeader.h>
#include "Intelligence/EUnitsFilter.h"
#include "Defines.h"
#include "Macro/BasesManager.h"
#include "Micro/Micro.h"
#include "Micro/Goals/GoalManager.h"
#include "Micro/Goals/DefendGoal.h"

using namespace BWAPI;
using namespace std;


EUnitsFilter::EUnitsFilter()
{
    //timeManager = & TimeManager::Instance();

    _interestingTypes.insert(UnitTypes::Protoss_Arbiter);
    _interestingTypes.insert(UnitTypes::Protoss_Arbiter_Tribunal);
    _interestingTypes.insert(UnitTypes::Protoss_Archon);
    _interestingTypes.insert(UnitTypes::Protoss_Assimilator);
    _interestingTypes.insert(UnitTypes::Protoss_Carrier);
    _interestingTypes.insert(UnitTypes::Protoss_Citadel_of_Adun);
    _interestingTypes.insert(UnitTypes::Protoss_Corsair);
    _interestingTypes.insert(UnitTypes::Protoss_Cybernetics_Core);
    _interestingTypes.insert(UnitTypes::Protoss_Dark_Archon);
    _interestingTypes.insert(UnitTypes::Protoss_Dark_Templar);
    _interestingTypes.insert(UnitTypes::Protoss_Dragoon);
    _interestingTypes.insert(UnitTypes::Protoss_Fleet_Beacon);
    _interestingTypes.insert(UnitTypes::Protoss_Forge);
    _interestingTypes.insert(UnitTypes::Protoss_Gateway);
    _interestingTypes.insert(UnitTypes::Protoss_High_Templar);
    _interestingTypes.insert(UnitTypes::Protoss_Nexus);
    _interestingTypes.insert(UnitTypes::Protoss_Observatory);
    _interestingTypes.insert(UnitTypes::Protoss_Observer);
    _interestingTypes.insert(UnitTypes::Protoss_Photon_Cannon);
    _interestingTypes.insert(UnitTypes::Protoss_Probe);
    _interestingTypes.insert(UnitTypes::Protoss_Pylon);
    _interestingTypes.insert(UnitTypes::Protoss_Reaver);
    _interestingTypes.insert(UnitTypes::Protoss_Robotics_Facility);
    _interestingTypes.insert(UnitTypes::Protoss_Robotics_Support_Bay);
    _interestingTypes.insert(UnitTypes::Protoss_Scout);
    _interestingTypes.insert(UnitTypes::Protoss_Shield_Battery);
    _interestingTypes.insert(UnitTypes::Protoss_Shuttle);
    _interestingTypes.insert(UnitTypes::Protoss_Stargate);
    _interestingTypes.insert(UnitTypes::Protoss_Templar_Archives);
    _interestingTypes.insert(UnitTypes::Protoss_Zealot);
    _interestingTypes.insert(UnitTypes::Terran_Academy);
    _interestingTypes.insert(UnitTypes::Terran_Armory);
    _interestingTypes.insert(UnitTypes::Terran_Barracks);
    _interestingTypes.insert(UnitTypes::Terran_Battlecruiser);
    _interestingTypes.insert(UnitTypes::Terran_Bunker);
    _interestingTypes.insert(UnitTypes::Terran_Command_Center);
    _interestingTypes.insert(UnitTypes::Terran_Comsat_Station);
    _interestingTypes.insert(UnitTypes::Terran_Control_Tower);
    _interestingTypes.insert(UnitTypes::Terran_Covert_Ops);
    _interestingTypes.insert(UnitTypes::Terran_Dropship);
    _interestingTypes.insert(UnitTypes::Terran_Engineering_Bay);
    _interestingTypes.insert(UnitTypes::Terran_Factory);
    _interestingTypes.insert(UnitTypes::Terran_Firebat);
    _interestingTypes.insert(UnitTypes::Terran_Ghost);
    _interestingTypes.insert(UnitTypes::Terran_Goliath);
    _interestingTypes.insert(UnitTypes::Terran_Machine_Shop);
    _interestingTypes.insert(UnitTypes::Terran_Marine);
    _interestingTypes.insert(UnitTypes::Terran_Medic);
    _interestingTypes.insert(UnitTypes::Terran_Missile_Turret);
    _interestingTypes.insert(UnitTypes::Terran_Nuclear_Silo);
    _interestingTypes.insert(UnitTypes::Terran_Physics_Lab);
    _interestingTypes.insert(UnitTypes::Terran_Refinery);
    _interestingTypes.insert(UnitTypes::Terran_Science_Facility);
    _interestingTypes.insert(UnitTypes::Terran_SCV);
    _interestingTypes.insert(UnitTypes::Terran_Siege_Tank_Siege_Mode);
    _interestingTypes.insert(UnitTypes::Terran_Siege_Tank_Tank_Mode);
    _interestingTypes.insert(UnitTypes::Terran_Starport);
    _interestingTypes.insert(UnitTypes::Terran_Supply_Depot);
    _interestingTypes.insert(UnitTypes::Terran_Valkyrie);
    _interestingTypes.insert(UnitTypes::Terran_Vulture);
    _interestingTypes.insert(UnitTypes::Terran_Vulture_Spider_Mine);
    _interestingTypes.insert(UnitTypes::Terran_Wraith);
    _interestingTypes.insert(UnitTypes::Zerg_Extractor);
    _interestingTypes.insert(UnitTypes::Zerg_Infested_Command_Center);
    _interestingTypes.insert(UnitTypes::Zerg_Hatchery);
    _interestingTypes.insert(UnitTypes::Zerg_Lair);
    _interestingTypes.insert(UnitTypes::Zerg_Hive);
    _interestingTypes.insert(UnitTypes::Zerg_Nydus_Canal);
    _interestingTypes.insert(UnitTypes::Zerg_Hydralisk_Den);
    _interestingTypes.insert(UnitTypes::Zerg_Defiler_Mound);
    _interestingTypes.insert(UnitTypes::Zerg_Greater_Spire);
    _interestingTypes.insert(UnitTypes::Zerg_Queens_Nest);
    _interestingTypes.insert(UnitTypes::Zerg_Evolution_Chamber);
    _interestingTypes.insert(UnitTypes::Zerg_Ultralisk_Cavern);
    _interestingTypes.insert(UnitTypes::Zerg_Spire);
    _interestingTypes.insert(UnitTypes::Zerg_Spawning_Pool);
    _interestingTypes.insert(UnitTypes::Zerg_Creep_Colony);
    _interestingTypes.insert(UnitTypes::Zerg_Spore_Colony);
    _interestingTypes.insert(UnitTypes::Zerg_Sunken_Colony);
    _interestingTypes.insert(UnitTypes::Zerg_Lurker);
    _interestingTypes.insert(UnitTypes::Zerg_Devourer);
    _interestingTypes.insert(UnitTypes::Zerg_Zergling);
    _interestingTypes.insert(UnitTypes::Zerg_Hydralisk);
    _interestingTypes.insert(UnitTypes::Zerg_Ultralisk);
    _interestingTypes.insert(UnitTypes::Zerg_Drone);
    _interestingTypes.insert(UnitTypes::Zerg_Overlord);
    _interestingTypes.insert(UnitTypes::Zerg_Mutalisk);
    _interestingTypes.insert(UnitTypes::Zerg_Guardian);
    _interestingTypes.insert(UnitTypes::Zerg_Queen);
    _interestingTypes.insert(UnitTypes::Zerg_Defiler);
    _interestingTypes.insert(UnitTypes::Zerg_Scourge);
    _interestingTypes.insert(UnitTypes::Zerg_Infested_Terran);
}

void EUnitsFilter::update(Unit* u)
{
    if (u->getPlayer() == Broodwar->self())
        return;
    if (u->getPlayer()->isNeutral())
        return;
    if (!_interestingTypes.count(u->getType()))
        return;
    if (_eViewedUnits.count(u))
        _eViewedUnits[u].update(Broodwar->getFrameCount());
    else 
        _eViewedUnits[u] = EViewedUnit(u, Broodwar->getFrameCount());
    if ((!(u->isDetected()) || u->isCloaked() || u->isBurrowed()) && Broodwar->isVisible(TilePosition(_eViewedUnits[u].position)))
    {
        _invisibleUnits[u] = std::make_pair<UnitType, Position>(u->getType(), u->getPosition());
#ifdef __INTELLIGENCE_DEBUG__
        Broodwar->printf("Type %s", _eViewedUnits[u].type.getName().c_str());
#endif
    }
}

void EUnitsFilter::filter(Unit* u)
{
    if (_eViewedUnits[u].type.isBuilding()) return; // we consider that buildings don't move
    if (_invisibleUnits.count(u) && (
        (u && u->exists() && ((!u->isCloaked() && !u->isBurrowed()) || u->isDetected())) 
        || Broodwar->getFrameCount() - _eViewedUnits[u].lastSeen > 216)) // remove units from invisible if we can see them after 9 secondes
    {
        _invisibleUnits.erase(u);
        return;
    }
    if (Broodwar->getFrameCount() - _eViewedUnits[u].lastSeen > 5760) // we forget units after 4 minutes 
        _eViewedUnits.erase(u);
}

void EUnitsFilter::onUnitDestroy(Unit* u)
{
    if (u->isVisible() && u->getPlayer() != Broodwar->enemy()) return;
    _eViewedUnits.erase(u);
    _invisibleUnits.erase(u);
}

void EUnitsFilter::onUnitMorph(Unit* u)
{
    update(u);
}

void EUnitsFilter::onUnitShow(Unit* u)
{
    update(u);
}

void EUnitsFilter::onUnitHide(Unit* u)
{
    update(u);
}

void EUnitsFilter::onUnitRenegade(Unit* u)
{
    update(u);
}

/*void EUnitsFilter::updateEArmies()
{
    map<Unit*, EViewedUnit> unassigned = map<Unit*, EViewedUnit>();
	unassigned.insert(_eViewedUnits.begin(), _eViewedUnits.end());
	int i = 0;
	list<Unit*> toRem;
	map<Unit*, int> tmpPairings = map<Unit*, int>();
	for (map<Unit*, EViewedUnit>::iterator it = unassigned.begin();
		it != unassigned.end(); ++it)
	{
		int current_ind = -1;
		if (tmpPairings.find(it->first) != tmpPairings.end())
			current_ind = tmpPairings[it->first];
		else
		{
			current_ind = i++;
			_eArmies.insert(pair<int, EArmy>(current_ind, EArmy()));
		}

		Vec tmpArmyCenter(0, 0);
		for each (pair<Unit*, EViewedUnit> eUnit2 in unassigned)
		{
			if (it->second.position.getApproxDistance(eUnit2.second.position) < TILE_SIZE * ARMY_RADIUS_CLUSTER)
			{
				tmpPairings.insert(pair<Unit*, int>(eUnit2.first, current_ind));
				_eArmies[current_ind].eUnits.push_back(eUnit2.first);
				tmpArmyCenter += eUnit2.second.position;
			}
		}
		_eArmies[current_ind].position = tmpArmyCenter.toPosition();

		toRem.push_back(it->first);
	}
	for each (Unit* u in toRem)
		unassigned.erase(u);
}*/

/*const std::map<int, EArmy>& EUnitsFilter::getEArmies()
{
	return _eArmies;
}*/

void EUnitsFilter::update()
{
	updateEArmies();
}

const std::map<BWAPI::Unit*, EViewedUnit>& EUnitsFilter::getViewedUnits()
{
    return _eViewedUnits;
}

EViewedUnit EUnitsFilter::getViewedUnit(Unit* u)
{
    return _eViewedUnits[u];
}

int EUnitsFilter::getNumbersType(UnitType ut)
{
    int count = 0;
    for (std::map<BWAPI::Unit*, EViewedUnit>::const_iterator it = _eViewedUnits.begin();
        it != _eViewedUnits.end(); ++it)
    {
        if (it->second.type == ut)
            ++count;
    }
    return count;
}

const std::map<Unit*, std::pair<UnitType, Position> >& EUnitsFilter::getInvisibleUnits()
{
    return _invisibleUnits;
}

bool EUnitsFilter::empty()
{
    return _eViewedUnits.empty();
}

BWAPI::Unit* EUnitsFilter::getClosestCenter(BWTA::BaseLocation* b)
{
	for each (std::pair<BWAPI::Unit*, EViewedUnit> pp in _eViewedUnits)
	{
		if (pp.second.type == Broodwar->enemy()->getRace().getCenter()
			&& pp.second.position.getApproxDistance(b->getPosition()) < 5*TILE_SIZE)
		{
			return pp.first;
		}
	}
	return NULL;
}

#ifdef __INTELLIGENCE_DEBUG__
void EUnitsFilter::bwOutput()
{
    for (std::map<BWAPI::Unit*, EViewedUnit>::const_iterator it = _eViewedUnits.begin(); 
        it != _eViewedUnits.end(); ++it)
        Broodwar->printf("Unit: %i", it->first);
}
#endif