#include "ScourgeUnit.h"

std::set<BWAPI::UnitType> ScourgeUnit::setPrio;

std::set<BWAPI::Unit*> ScourgeUnit::alreadyTargeted;

ScourgeUnit::ScourgeUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_Corsair);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
        setPrio.insert(BWAPI::UnitTypes::Terran_Valkyrie);
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
        for (std::multimap<double, BWAPI::Unit*>::const_iterator it = _rangeEnemies.begin();
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
                for (std::multimap<double, BWAPI::Unit*>::const_iterator it = _rangeEnemies.begin();
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
        return Broodwar->getLatency() + 3; // (targetEnemy->getDistance(_unitPos))/(unit->getType().topSpeed())
    else
        return Broodwar->getLatency();
}

std::set<BWAPI::UnitType> ScourgeUnit::getSetPrio()
{
    return ScourgeUnit::setPrio;
}