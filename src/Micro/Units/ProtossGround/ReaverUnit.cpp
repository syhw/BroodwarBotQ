#include <PrecompiledHeader.h>
#include "ReaverUnit.h"

std::set<BWAPI::UnitType> ReaverUnit::setPrio;

ReaverUnit::ReaverUnit(BWAPI::Unit* u, UnitsGroup* ug)
: GroundUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Defiler);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Dragoon);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Reaver);
        setPrio.insert(BWAPI::UnitTypes::Terran_Goliath);
        setPrio.insert(BWAPI::UnitTypes::Terran_SCV);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Drone);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Probe);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Hydralisk);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Lurker);
    }
}

ReaverUnit::~ReaverUnit()
{
}

void ReaverUnit::micro()
{
    if (unit->getScarabCount() && (Broodwar->getFrameCount() - _lastAttackFrame) > (Broodwar->getLatency() + getAttackDuration()))
    {
        updateRangeEnemies();
        updateTargetEnemy();
        unit->attack(targetEnemy);
        _lastAttackFrame = Broodwar->getFrameCount();
    } else if (!(Broodwar->getFrameCount() % 5))
    {
        updateRangeEnemies();
        updateTargetEnemy();
    }
}

void ReaverUnit::check()
{
    if (unit->getScarabCount() == 0)
    {
        unit->train(UnitTypes::Protoss_Scarab);
        unit->train(UnitTypes::Protoss_Scarab);
        unit->train(UnitTypes::Protoss_Scarab);
    }
    if (unit->getScarabCount() < 4 && !(unit->isTraining()))
        unit->train(UnitTypes::Protoss_Scarab);
}

bool ReaverUnit::inRange(BWAPI::Unit* u)
{
    return (_unitPos.getDistance(u->getPosition()) <= (double)8*32);
}

int ReaverUnit::getAttackDuration()
{
    return 60;
}

std::set<BWAPI::UnitType> ReaverUnit::getSetPrio()
{
    return ReaverUnit::setPrio;
}