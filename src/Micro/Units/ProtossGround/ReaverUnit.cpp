#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossGround/ReaverUnit.h"

using namespace BWAPI;

std::set<UnitType> ReaverUnit::setPrio;

ReaverUnit::ReaverUnit(Unit* u)
: GroundUnit(u)
{
    if (setPrio.empty())
    {
        setPrio.insert(UnitTypes::Protoss_High_Templar);
        setPrio.insert(UnitTypes::Zerg_Defiler);
        setPrio.insert(UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(UnitTypes::Terran_Siege_Tank_Tank_Mode);
        setPrio.insert(UnitTypes::Protoss_Dragoon);
        setPrio.insert(UnitTypes::Protoss_Reaver);
        setPrio.insert(UnitTypes::Terran_SCV);
        setPrio.insert(UnitTypes::Zerg_Drone);
        setPrio.insert(UnitTypes::Protoss_Probe);
        setPrio.insert(UnitTypes::Zerg_Hydralisk);
        setPrio.insert(UnitTypes::Zerg_Lurker);
    }
}

ReaverUnit::~ReaverUnit()
{
}

void ReaverUnit::micro()
{
    if (unit->getScarabCount() && (Broodwar->getFrameCount() - _lastAttackFrame) > (Broodwar->getLatencyFrames() + getAttackDuration()))
    {
        updateRangeEnemies();
        updateTargetEnemy();
        unit->attack(targetEnemy);
        _lastAttackFrame = Broodwar->getFrameCount();
    }
}

void ReaverUnit::check()
{
	if (unit->getTrainingQueue().size() > 1 || Broodwar->getFrameCount() <= Broodwar->getLatencyFrames() + _lastClickFrame)
		return;
    if (unit->getScarabCount() < 4 && !(unit->isTraining()))
	{
        unit->train(UnitTypes::Protoss_Scarab);
		_lastClickFrame = Broodwar->getFrameCount();
	}
}

bool ReaverUnit::inRange(Unit* u)
{
    return (!u->getType().isFlyer() && _unitPos.getDistance(u->getPosition()) <= (double)8*32);
}

int ReaverUnit::getAttackDuration()
{
    return 42; // real cooldown is 60 :)
}

std::set<UnitType> ReaverUnit::getSetPrio()
{
    return ReaverUnit::setPrio;
}