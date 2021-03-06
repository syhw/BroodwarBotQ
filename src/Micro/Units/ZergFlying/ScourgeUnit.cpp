#include <PrecompiledHeader.h>
#include "Micro/Units/ZergFlying/ScourgeUnit.h"

using namespace BWAPI;

std::set<UnitType> ScourgeUnit::setPrio;

std::set<Unit*> ScourgeUnit::alreadyTargeted;

ScourgeUnit::ScourgeUnit(Unit* u)
: FlyingUnit(u)
{
    if (setPrio.empty())
    {
        setPrio.insert(UnitTypes::Protoss_Corsair);
        setPrio.insert(UnitTypes::Zerg_Mutalisk);
        setPrio.insert(UnitTypes::Terran_Valkyrie);
    }
}

ScourgeUnit::~ScourgeUnit()
{
    alreadyTargeted.erase(targetEnemy);
}

void ScourgeUnit::micro()
{
    if (targetEnemy != NULL && !(targetEnemy->exists()))
    {
        updateRangeEnemies();
        for (std::multimap<double, Unit*>::const_iterator it = _rangeEnemies.begin();
            it != _rangeEnemies.end(); ++it)
        {
            if (alreadyTargeted.count(it->second))
                continue;
            if (it->second->isVisible() && it->second->getType() != UnitTypes::Zerg_Mutalisk)
                continue;
            alreadyTargeted.insert(it->second);
            targetEnemy = it->second;
        }
        unit->rightClick(targetEnemy);
        _lastAttackFrame = Broodwar->getFrameCount();
    }
    else
    {
        if (Broodwar->getFrameCount() - _lastAttackFrame > getAttackDuration())
        {
            if (unit->getGroundWeaponCooldown() == 0)
            {
                updateRangeEnemies();
                for (std::multimap<double, Unit*>::const_iterator it = _rangeEnemies.begin();
                    it != _rangeEnemies.end(); ++it)
                {
                    if (alreadyTargeted.count(it->second))
                        continue;
                    if (it->second->isVisible() && it->second->getType() != UnitTypes::Zerg_Mutalisk)
                        continue;
                    if (!targetEnemy || !targetEnemy->exists())
                    {
                        alreadyTargeted.insert(it->second);
                        targetEnemy = it->second;
                    }
                    if (targetEnemy && targetEnemy->exists() && targetEnemy != it->second && Broodwar->getFrameCount() - _lastClickFrame > 42) // THIS IS THE ANSWER
                    {
                        alreadyTargeted.erase(targetEnemy);
                        alreadyTargeted.insert(it->second);
                        targetEnemy = it->second;
                        _lastClickFrame = Broodwar->getFrameCount();
                    }
                }
                unit->rightClick(targetEnemy);
                _lastAttackFrame = Broodwar->getFrameCount();
            }
        }
    }
}

void ScourgeUnit::check()
{
}

int ScourgeUnit::getAttackDuration()
{
    if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible())
        return Broodwar->getLatencyFrames() + 3; // (targetEnemy->getDistance(_unitPos))/(unit->getType().topSpeed())
    else
        return Broodwar->getLatencyFrames();
}

std::set<UnitType> ScourgeUnit::getSetPrio()
{
    return ScourgeUnit::setPrio;
}