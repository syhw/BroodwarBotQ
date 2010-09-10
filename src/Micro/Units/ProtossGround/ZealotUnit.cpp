#include "ZealotUnit.h"
#include <BWAPI.h>
#include <UnitsGroup.h>
#include "MapManager.h"

using namespace std;
using namespace BWAPI;

std::set<BWAPI::UnitType> ZealotUnit::setPrio;

ZealotUnit::ZealotUnit(BWAPI::Unit* u, UnitsGroup* ug)
: GroundUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_Reaver);
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Hydralisk);
    }
    _fleeingDmg = 14; // storm 
}

ZealotUnit::~ZealotUnit()
{
}

void ZealotUnit::micro()
{
    if (unit->isStartingAttack())
    {
        //Broodwar->printf("is starting attack: %d, attack freq %d", Broodwar->getFrameCount(), unit->getType().groundWeapon().damageCooldown());
        _lastAttackFrame = Broodwar->getFrameCount();
        return;
    }
    updateRangeEnemies();
    updateTargetEnemy();
            if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible() && inRange(targetEnemy))
            attackEnemyUnit(targetEnemy);
        else if (oorTargetEnemy && oorTargetEnemy->exists() && oorTargetEnemy->isVisible() && oorTargetEnemy->getDistance(_unitPos) < 92.0)
            attackEnemyUnit(oorTargetEnemy);
        else if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible())
            attackEnemyUnit(targetEnemy);
        else
            unit->attackMove(_unitsGroup->enemiesCenter);
    const std::map<Bullet*, Position>& storms = MapManager::Instance().getTrackedStorms();
    for (std::map<Bullet*, Position>::const_iterator it = storms.begin();
        it != storms.end(); ++it)
    {
        if (it->second.getApproxDistance(_unitPos) < 67.8)
        {
            // update possible directions vector
            updateDirV();
            // update attractions
            updateAttractors();
            // update objectives
            if (oorTargetEnemy && oorTargetEnemy->exists() && oorTargetEnemy->isVisible())
            {
                obj = Vec(oorTargetEnemy->getPosition().x() - _unitPos.x(), oorTargetEnemy->getPosition().y() - _unitPos.y());
                Broodwar->printf("here");
            }
            else if (_unitsGroup->enemiesCenter != Positions::None)
            {
                obj = Vec(_unitsGroup->enemiesCenter.x() - _unitPos.x(), _unitsGroup->enemiesCenter.y() - _unitPos.y());
            }
            else
                obj = Vec(0, 0);

            // compute the probability to go in each dirv(ector)
            computeProbs();
            // select the most probable, most in the direction of obj if equally probables
            selectDir(obj);
            clickDir();
            return;
        }
    }
    if (Broodwar->getFrameCount() - _lastAttackFrame > Broodwar->getLatency()) //&& Broodwar->getFrameCount() - _lastAttackFrame + Broodwar->getLatency() < unit->getType().groundWeapon().damageCooldown())
    {
        /// ATTACK
        if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible() && inRange(targetEnemy))
            attackEnemyUnit(targetEnemy);
        else if (oorTargetEnemy && oorTargetEnemy->exists() && oorTargetEnemy->isVisible() && oorTargetEnemy->getDistance(_unitPos) < 92.0)
            attackEnemyUnit(oorTargetEnemy);
        else if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible())
            attackEnemyUnit(targetEnemy);
        else
            unit->attackMove(_unitsGroup->enemiesCenter);
    }
    /*
    int hpDiff = _lastTotalHP - (unit->getShields() + unit->getHitPoints()) + 1; // +1 for regen shield
    if (Broodwar->getFrameCount() - _lastAttackFrame > Broodwar->getLatency() + getAttackDuration())
    {
        if (unit->getGroundWeaponCooldown() == 0)
        {
            updateRangeEnemies();
            updateTargetEnemy();
            attackEnemyUnit(targetEnemy);
        }
        else if (unit->getGroundWeaponCooldown() <= Broodwar->getLatency())
        {
            updateRangeEnemies();
            updateTargetEnemy();
            attackEnemyUnit(targetEnemy);
        }
        else if (hpDiff >= _fleeingDmg || (unit->getShields() == 0 && hpDiff > 5))
        {
            Vec fleeObj = Vec(0, 0);
            for (std::map<Unit*, Position>::const_iterator it = _unitsGroup->enemies.begin();
                it != _unitsGroup->enemies.end(); ++it)
            {
                if (it->first && it->first->exists() && it->first->isVisible()
                    && it->first->getTarget() == unit
                    && (it->first->getType() == UnitTypes::Zerg_Zergling 
                    || it->first->getType() == UnitTypes::Protoss_Zealot))
                {
                    Vec tmp = Vec(_unitPos.x() - it->second.x(), _unitPos.y() - it->second.y());
                    tmp.normalize();
                    fleeObj += tmp;
                }
            }
            if (fleeObj.norm() > 1.1) // at least 2 units are targeting us
            {
                fleeObj.normalize();
                fleeObj *= 16;
                Position p = fleeObj.translate(_unitPos);
                if (Broodwar->isWalkable(p.x()/8, p.y()/8))
                {
                    if (!p.isValid())
                        p.makeValid();
                    unit->move(p);
                }
            }
            else if (fleeObj.norm() < 0.9) // no unit is targeting us
            {
                flee();
            }
        }
        else if (!unit->isMoving() && targetEnemy != NULL)
        {
            fightMove();
        }
    }
    */
}

void ZealotUnit::check()
{
    if (unit->getUpgradeLevel(UpgradeTypes::Leg_Enhancements) && !setPrio.count(UnitTypes::Terran_Siege_Tank_Siege_Mode))
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
    }
}

int ZealotUnit::getAttackDuration()
{
    return 1;
}

std::set<BWAPI::UnitType> ZealotUnit::getSetPrio()
{
    return ZealotUnit::setPrio;
}