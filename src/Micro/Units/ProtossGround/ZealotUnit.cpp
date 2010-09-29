#include "ZealotUnit.h"
#include <BWAPI.h>
#include <UnitsGroup.h>
#include "MapManager.h"

using namespace std;
using namespace BWAPI;

std::set<BWAPI::UnitType> ZealotUnit::setPrio;
std::map<BWAPI::Unit*, int> ZealotUnit::_zealotsOn;

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
    _fleeingDmg = 36; // one round of storm = 14
}

ZealotUnit::~ZealotUnit()
{
    if (Broodwar->getFrameCount() - _lastAttackFrame <= getAttackDuration())
        clearDamages();
}

bool ZealotUnit::decideToFlee()
{
    if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible() 
        && Broodwar->getGroundHeight(targetEnemy->getPosition()) > Broodwar->getGroundHeight(_unitPos))
    {
        if (_unitsGroup->nearestChoke && _unitsGroup->nearestChoke->getCenter().getDistance(_unitPos) < 128)
        {
            _fleeing = false;
            return false;
        }
    }
    if (unit->getShields() < 10)
        _fleeingDmg = 24;
    // TODO complete conditions
    int diff = _lastTotalHP - (unit->getShields() + unit->getHitPoints());
    _HPLosts.push_back(diff);
    _sumLostHP += diff;
    if (_HPLosts.size() > 19)
    {
        _sumLostHP -= _HPLosts.front();
        _HPLosts.pop_front();
    }
    if (_sumLostHP > _fleeingDmg)
        _fleeing = true;
    else
        _fleeing = false;
    if (!_fleeing)
    {
        int incDmg = 0;
        for (std::set<Unit*>::const_iterator it = _targetingMe.begin();
            it != _targetingMe.end(); ++it)
        {
            if ((*it)->getDistance(_unitPos) <= (*it)->getType().groundWeapon().maxRange() + 16)
                incDmg += (*it)->getType().groundWeapon().damageAmount() * (*it)->getType().maxGroundHits();
        }
        if (incDmg + _sumLostHP > _fleeingDmg)
            _fleeing = true;
    }
    return _fleeing;
}

void ZealotUnit::clearDamages()
{
    // ===== clear old damage =====
    if (targetEnemy != NULL && _unitsGroup->unitDamages.left.count(targetEnemy))
    {
        if (_zealotsOn.count(targetEnemy))
        {
            if (_zealotsOn[targetEnemy] <= 1)
                _zealotsOn.erase(targetEnemy);
            else
                _zealotsOn[targetEnemy] = _zealotsOn[targetEnemy] - 1;
        }

        UnitDmgBimap::left_iterator unitdmgit = _unitsGroup->unitDamages.left.find(targetEnemy);
        int tmp_dmg = unitdmgit->second.dmg - computeDmg(targetEnemy);
        Unit* tmp_unit = unitdmgit->first;
        _unitsGroup->unitDamages.left.erase(unitdmgit);
        _unitsGroup->unitDamages.insert(UnitDmg(tmp_unit, Dmg(tmp_dmg, tmp_unit, 
            (targetEnemy->exists() && targetEnemy->isVisible() && targetEnemy->isDetected() ? tmp_unit->getHitPoints() + tmp_unit->getShields() : 0))));
    }
}

void ZealotUnit::setTargetEnemy(Unit* u)
{
    if (oorTargetEnemy == u)
        oorTargetEnemy = NULL; // not to move in its direction as we are already in range

    if (_zealotsOn.count(u))
        _zealotsOn[u] = _zealotsOn[u] + 1;
    else
        _zealotsOn.insert(std::make_pair<Unit*, int>(u, 1));

    targetEnemy = u;
    if (u != NULL && _unitsGroup->unitDamages.left.count(u))
    { /// <=> _unitsGroup->unitDamages.left[u] += computeDmg(u);
        UnitDmgBimap::left_iterator it = _unitsGroup->unitDamages.left.find(u);
        int tmp_dmg = it->second.dmg + computeDmg(u);
        _unitsGroup->unitDamages.left.erase(it);
        _unitsGroup->unitDamages.insert(UnitDmg(u, Dmg(tmp_dmg, u, 
            (u->exists() && u->isVisible() && u->isDetected() ? u->getHitPoints() + u->getShields() : 0))));
    }
    else
        _unitsGroup->unitDamages.insert(UnitDmg(u, Dmg(computeDmg(u), u, 
        (u->exists() && u->isVisible() && u->isDetected() ? u->getHitPoints() + u->getShields() : 0))));
}

void ZealotUnit::updateTargetEnemy()
{
    // ===== update oorTargetEnemy =====
    /// take one in our priority set and in focus fire order
    for (UnitDmgBimap::right_iterator it = _unitsGroup->unitDamages.right.begin();
        it != _unitsGroup->unitDamages.right.end(); ++it)
    {
        if (!it->second->exists() || !it->second->isVisible())
            continue;
        if (inRange(it->second))
            continue;
        UnitType testType = it->second->getType();
        if (testType.isBuilding() 
            && testType != BWAPI::UnitTypes::Protoss_Pylon
            && testType != BWAPI::UnitTypes::Protoss_Photon_Cannon
            && testType != BWAPI::UnitTypes::Terran_Bunker
            && testType != BWAPI::UnitTypes::Terran_Missile_Turret
            && testType != BWAPI::UnitTypes::Zerg_Sunken_Colony
            && testType != BWAPI::UnitTypes::Zerg_Spore_Colony)
            continue;
        if (testType == BWAPI::UnitTypes::Protoss_High_Templar && it->second->getEnergy() < 60)
            continue;
        if (testType == BWAPI::UnitTypes::Protoss_Dark_Archon && it->second->getEnergy() < 80)
            continue;
        if (testType == BWAPI::UnitTypes::Terran_Science_Vessel && it->second->getEnergy() < 70)
            continue;
        if (testType == BWAPI::UnitTypes::Zerg_Defiler && it->second->getEnergy() < 60)
            continue;
        if (testType == BWAPI::UnitTypes::Zerg_Queen && it->second->getEnergy() < 60)
            continue;
        if (getSetPrio().count(testType)
            && it->first.dmg < it->second->getHitPoints() + it->second->getShields())
        {
            oorTargetEnemy = it->second;
            break;
        }
    }
    if (oorTargetEnemy == NULL || !oorTargetEnemy->exists())
    {
        /// take one in our priority set
        for (std::multimap<double, BWAPI::Unit*>::const_iterator it = _rangeEnemies.begin();
            it != _rangeEnemies.end(); ++it)
        {
            if (!it->second->exists() || !it->second->isVisible())
                continue;
            if (inRange(it->second))
                continue;
            UnitType testType = it->second->getType();
            if (testType == BWAPI::UnitTypes::Protoss_Interceptor)
                continue;
            if (testType.isBuilding() 
                && testType != BWAPI::UnitTypes::Protoss_Pylon
                && testType != BWAPI::UnitTypes::Protoss_Photon_Cannon
                && testType != BWAPI::UnitTypes::Terran_Bunker
                && testType != BWAPI::UnitTypes::Terran_Missile_Turret
                && testType != BWAPI::UnitTypes::Zerg_Sunken_Colony
                && testType != BWAPI::UnitTypes::Zerg_Spore_Colony)
                continue;
            if (getSetPrio().count(testType))
            {
                oorTargetEnemy = it->second;
                break;
            }
        }
    }

    // ===== choose new targetEnemy =====
    /// 2 zealots by target
    for (std::multimap<double, Unit*>::const_iterator it = _rangeEnemies.begin();
        it != _rangeEnemies.end(); ++it)
    {
        if (_zealotsOn[it->second] > 2)
            continue;
        setTargetEnemy(it->second);
        return;
    }
    for (std::multimap<double, Unit*>::const_iterator it = _rangeEnemies.begin();
        it != _rangeEnemies.end(); ++it)
    {
        setTargetEnemy(it->second);
        return;
    }

    /// Take the default target of the UnitGroup
    if (_unitsGroup->defaultTargetEnemy != NULL && _unitsGroup->defaultTargetEnemy->exists())
        setTargetEnemy(_unitsGroup->defaultTargetEnemy);
    /// No target in rangeEnemies nor as a default target, follow the old one          // should never happen
    else if (targetEnemy != NULL && targetEnemy->exists() && targetEnemy->isVisible()) // should never happen
        setTargetEnemy(targetEnemy);                                                   // should never happen
    /// WTF???                                                                         // should never happen
    else                                                                               // should never happen
        Broodwar->printf("WTF? Mode fight, no enemy");                                 // should never happen
}


void ZealotUnit::micro()
{
    updateTargetingMe();
    decideToFlee();
    int currentFrame = Broodwar->getFrameCount();
    if (unit->isStartingAttack())
        _lastAttackFrame = currentFrame;
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attacks
        return;
    if (currentFrame - _lastAttackFrame == getAttackDuration() + 1)
        clearDamages();
    updateRangeEnemies();
    if (Broodwar->enemy()->getRace() == BWAPI::Races::Zerg) ////////////////////////// AttackMove vs Zerg
        updateTargetEnemy();

    /// Dodge storm, drag mine, drag scarab
    if (dodgeStorm() || dragMine() || dragScarab()) 
        return;

    if (Broodwar->enemy()->getRace() == BWAPI::Races::Zerg) ////////////////////////// AttackMove vs Zerg
    {
        if (currentFrame - _lastClickFrame > 21)
        {
            if (target == _unitPos)
                unit->attackMove(_unitsGroup->enemiesCenter);
            else
                unit->attackMove(target);
            _lastMoveFrame = Broodwar->getFrameCount();
            _lastClickFrame = Broodwar->getFrameCount();
        }
    }
    else
    {

        if (unit->getGroundWeaponCooldown() <= Broodwar->getLatency() + 1)
        {
            if (!inRange(targetEnemy))
            {
                clearDamages();
            }
            /// ATTACK
            if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible() && inRange(targetEnemy))
                attackEnemyUnit(targetEnemy);
            else if (oorTargetEnemy && oorTargetEnemy->exists() && oorTargetEnemy->isVisible() && oorTargetEnemy->getDistance(_unitPos) < 92.0) // TOCHANGE 92
                attackEnemyUnit(oorTargetEnemy);
            else if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible())
                attackEnemyUnit(targetEnemy);
            else
            {
                if (currentFrame - _lastClickFrame > Broodwar->getLatency())
                {
                    unit->attackMove(_unitsGroup->enemiesCenter);
                    _lastMoveFrame = Broodwar->getFrameCount();
                    _lastClickFrame = Broodwar->getFrameCount();
                }
            }
        }
        else if (unit->getGroundWeaponCooldown() > Broodwar->getLatency()*2 + 2) // == (Broodwar->getLatency()+1)*2, safety
        {
            if (_fleeing)
            {
                simpleFlee();
            }
            else
            {
                fightMove();
            }
        }
    }
    return;

    /*if (Broodwar->getFrameCount() - _lastAttackFrame <= Broodwar->getLatency() + 1) //&& Broodwar->getFrameCount() - _lastAttackFrame + Broodwar->getLatency() < unit->getType().groundWeapon().damageCooldown())
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
    }*/
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