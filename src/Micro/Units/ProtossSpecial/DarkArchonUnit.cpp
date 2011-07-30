#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossSpecial/DarkArchonUnit.h"

using namespace std;
using namespace BWAPI;

set<UnitType> DarkArchonUnit::casters;
set<UnitType> DarkArchonUnit::setPrio;

DarkArchonUnit::DarkArchonUnit(Unit* u)
: SpecialUnit(u)
, _lastCastFrame(0)
{
	if (casters.empty())
	{
        casters.insert(UnitTypes::Zerg_Defiler);
		casters.insert(UnitTypes::Zerg_Queen);
        casters.insert(UnitTypes::Protoss_High_Templar);
        casters.insert(UnitTypes::Protoss_Arbiter);
        casters.insert(UnitTypes::Protoss_Dark_Archon);
		casters.insert(UnitTypes::Terran_Ghost);
		casters.insert(UnitTypes::Terran_Science_Vessel);
	}
	if (setPrio.empty())
    {
        setPrio.insert(UnitTypes::Zerg_Lurker);
		setPrio.insert(UnitTypes::Zerg_Ultralisk);
        setPrio.insert(UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(UnitTypes::Terran_BattleCruiser);
        setPrio.insert(UnitTypes::Protoss_Carrier);
        setPrio.insert(UnitTypes::Protoss_Archon);
        setPrio.insert(UnitTypes::Protoss_Reaver);
    }
}

DarkArchonUnit::~DarkArchonUnit()
{
}

void DarkArchonUnit::micro()
{ 
    int elapsed = Broodwar->getFrameCount() - _lastCastFrame;
    if (elapsed <= Broodwar->getLatencyFrames() + getAttackDuration())
        return;
	if (dodgeStorm() || dragMine())
        return;
	updateRangeEnemies();
	if (Broodwar->self()->hasResearched(TechTypes::Mind_Control)
		&& unit->getEnergy() > 149)
	{
		for (multimap<double, Unit*>::const_iterator it = _rangeEnemies.begin();
			it != _rangeEnemies.end(); ++it)
		{
			if (setPrio.count(it->second->getType())
				&& it->second->getHitPoints()+it->second->getShields() > 80
				&& !it->second->isHallucination()
				&& !it->second->isLockedDown()
				&& !it->second->isMaelstrommed()
				&& !it->second->isStasised())
			{
				unit->useTech(TechTypes::Mind_Control, it->second);
				_lastCastFrame = Broodwar->getFrameCount();
				return;
			}
		}
	}
    if (unit->getEnergy() > 49)
	{
		for (multimap<double, Unit*>::const_iterator it = _rangeEnemies.begin();
			it != _rangeEnemies.end(); ++it)
		{
			if (casters.count(it->second->getType()
				&& it->second->getEnergy() > it->second->getHitPoints()+it->second->getShields())
			{
				unit->useTech(TechTypes::Feedback, it->second);
				_lastCastFrame = Broodwar->getFrameCount();
				return;
			}
		}
	}
	if (Broodwar->self()->hasResearched(TechTypes::Maelstrom)
		&& unit->getEnergy() > 99)
	{
		for (multimap<double, Unit*>::const_iterator it = _rangeEnemies.begin();
			it != _rangeEnemies.end(); ++it)
		{
			if (it->second->getHitPoints() > 40 
				&& (setPrio.count(it->second->getType())
				    || it->second->getType() == UnitTypes::Zerg_Mutalisk)
				&& !it->second->isDefenseMatrixed()
				&& !it->second->isHallucination()
				&& !it->second->isLockedDown()
				&& !it->second->isMaelstrommed()
				&& !it->second->isStasised()
			{
				unit->useTech(TechTypes::Maelstrom, it->second);
				_lastCastFrame = Broodwar->getFrameCount();
				return;
			}
		}
	}
    if (this->unit->getEnergy() > 49)
	{
		for (multimap<double, Unit*>::const_iterator it = _rangeEnemies.begin();
			it != _rangeEnemies.end(); ++it)
		{
			if (casters.count(it->second->getType()
				&& it->second->getEnergy() > 60))
			{
				unit->useTech(TechTypes::Feedback, it->second);
				_lastCastFrame = Broodwar->getFrameCount();
				return;
			}
		}
	}
	fightMove(); // TODO check that / test it
}

void DarkArchonUnit::check()
{
}

int DarkArchonUnit::getAttackDuration()
{
    return 3;
}

set<UnitType> DarkArchonUnit::getSetPrio()
{
    return set<UnitType>();
}