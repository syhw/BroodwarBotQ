#include "DragoonUnit.h"
#include <BWAPI.h>
#include <UnitsGroup.h>

using namespace std;
using namespace BWAPI;

int DragoonUnit::addRange;
int DragoonUnit::attackDuration;

std::set<BWAPI::UnitType> DragoonUnit::setPrio;

DragoonUnit::DragoonUnit(BWAPI::Unit* u, UnitsGroup* ug)
: GroundUnit(u, ug)
{
    if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
        addRange = 64;
    else
        addRange = 0;
    attackDuration = 9;

    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Terran_Vulture_Spider_Mine);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Lurker);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Observer);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Carrier);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Defiler);
    }
    _fleeingDmg = 22; // 
}

DragoonUnit::~DragoonUnit()
{
    if (Broodwar->getFrameCount() - _lastAttackFrame <= getAttackDuration())
        clearDamages();
}

int DragoonUnit::addRangeGround()
{
    return addRange;
}

int DragoonUnit::addRangeAir()
{
    return addRange;
}

bool DragoonUnit::decideToFlee()
{
    // TODO complete conditions
    int diff = _lastTotalHP - (unit->getShields() + unit->getHitPoints());
    _HPLosts.push_back(diff);
    _sumLostHP += diff;
    if (_HPLosts.size() > 24)
    {
        _sumLostHP -= _HPLosts.front();
        _HPLosts.pop_front();
    }
    if (_sumLostHP > _fleeingDmg)
        _fleeing = true;
    else
        _fleeing = false;
    return _sumLostHP > _fleeingDmg;
}

void DragoonUnit::simpleFlee()
{
    _fightMoving = false;
    if (!this->mapManager->groundDamages[_unitPos.x()/32 + _unitPos.y()/32*Broodwar->mapWidth()])
    {
        _fleeing = false;
        return;
    }
    _fleeing = true;
    if (Broodwar->getFrameCount() - _lastClickFrame < Broodwar->getLatency())
        return;
    Vec dirFlee = Vec(0, 0);
    for (std::map<Unit*, Position>::const_iterator it = _unitsGroup->enemies.begin();
        it != _unitsGroup->enemies.end(); ++it)
    {
        if (it->first && it->first->exists() && it->first->isVisible()
            && (it->first->getTarget() == unit || it->first->getOrderTarget() == unit))// && it->first->getType().groundWeapon().maxRange() - it->second.getDistance(_unitPos) < unit->getType().topSpeed()*15)
        {
            dirFlee += Vec(_unitPos.x() - it->first->getPosition().x(), _unitPos.y() - it->first->getPosition().y());
        }
    }
    if (dirFlee != Vec(0, 0))
    dirFlee.normalize();
    dirFlee *= 64;
    Position tmp = dirFlee.translate(_unitPos);
    for (std::set<Unit*>::const_iterator it = Broodwar->self()->getUnits().begin();
        it != Broodwar->self()->getUnits().end(); ++it)
    {
        double dist = (*it)->getDistance(tmp);
        if (dist < 32.0)
        {
            if (dist < 16.0)
            {
                Vec tmpVec = Vec(tmp.x() - (*it)->getPosition().x(), tmp.y() - (*it)->getPosition().y());
                tmpVec.normalize();
                tmpVec *= 16;
                tmp = tmpVec.translate(tmp);
            }
            else
                tmp = Vec(tmp.x() - (*it)->getPosition().x(), tmp.y() - (*it)->getPosition().y()).translate(tmp);
        }
    }
    unit->move(tmp);
    //dir = dirFlee;
    //clickDir();
}

void DragoonUnit::micro()
{
#ifdef __DEBUG_GABRIEL__
    if (unit->isStartingAttack())
    {
        if (Broodwar->getSelectedUnits().count(unit))
            Broodwar->printf("starting attack at frame: %d, distance to target %f", Broodwar->getFrameCount(), targetEnemy->getDistance(unit));
    }
#endif
    decideToFlee();
    int currentFrame = Broodwar->getFrameCount();
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attacks
        return;
    if (currentFrame - _lastAttackFrame == getAttackDuration() + 1)
        clearDamages();
    /*if (_fleeing)
    {
        simpleFlee();
        return;
    }*/
    if (unit->getGroundWeaponCooldown() <= Broodwar->getLatency() + 1)
    {
        updateRangeEnemies();
        updateTargetEnemy();
        if (!inRange(targetEnemy))
        {
            clearDamages();
        }
        attackEnemyUnit(targetEnemy);
    }
    else if (unit->getGroundWeaponCooldown() > Broodwar->getLatency() + 2) // == (Broodwar->getLatency()+1)*2, safety
    {
        if(_fleeing)
        {
            simpleFlee();
        }
        else
        {
            fightMove();
        }
    }
}

void DragoonUnit::check()
{
    if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
        addRange = 64;
}

double DragoonUnit::getMaxRange()
{
    return BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Singularity_Charge) == 1 ? 192.0 : 128.0;
}

int DragoonUnit::getAttackDuration()
{
    return attackDuration;
}

std::set<BWAPI::UnitType> DragoonUnit::getSetPrio()
{
    return DragoonUnit::setPrio;
}