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
, _startedAttack(-100)
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
    _fleeingDmg = 12; // 
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
    return _sumLostHP > _fleeingDmg;
}

void DragoonUnit::simpleFlee()
{
    Vec dirFlee = Vec(0, 0);
    for (std::map<Unit*, Position>::const_iterator it = _unitsGroup->enemies.begin();
        it != _unitsGroup->enemies.end(); ++it)
    {
        if (it->first && it->first->exists() && it->first->isVisible()
            && it->first->getTarget() == unit )// && it->first->getType().groundWeapon().maxRange() - it->second.getDistance(_unitPos) < unit->getType().topSpeed()*15)
        {
            dirFlee += Vec(_unitPos.x() - it->first->getPosition().x(), _unitPos.y() - it->first->getPosition().y());
        }
    }
    if (dirFlee != Vec(0, 0))
    dirFlee.normalize();
    dirFlee *= 64;
    Position tmp = dirFlee.translate(_unitPos);
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
            Broodwar->printf("starting attack at frame: %d, distance to target %d", Broodwar->getFrameCount(), targetEnemy->getDistance(unit));
    }
#endif
    if (unit->isStartingAttack())
        _startedAttack = Broodwar->getFrameCount();
    decideToFlee();
    int currentFrame = Broodwar->getFrameCount();
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attacks
        return;
    if (currentFrame - _lastAttackFrame == getAttackDuration() + 1)
        clearDamages();
    if (unit->getGroundWeaponCooldown() <= Broodwar->getLatency() + 1)
    {
        updateRangeEnemies();
        updateTargetEnemy();
        if (Broodwar->getFrameCount() - _startedAttack > getAttackDuration())
            attackEnemyUnit(targetEnemy);
        //if (unit->getOrderTarget() != targetEnemy)
        //    attackEnemyUnit(targetEnemy);
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