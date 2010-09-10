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
    _fleeingDmg = 14;
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
    // TODO integrate over periods of time
    _fleeingDmg = 4;
    return (_lastTotalHP - (unit->getShields() + unit->getHitPoints()) >= _fleeingDmg || (!unit->getShields() && (_lastTotalHP - unit->getHitPoints() > 3)));
}

void DragoonUnit::micro()
{
#ifdef __DEBUG_GABRIEL__
    if (unit->isStartingAttack())
    {
        if (Broodwar->getSelectedUnits().count(unit))
            Broodwar->printf("starting attack at frame: %d", Broodwar->getFrameCount());
    }
#endif
    if (!_fleeing)
        _fleeing = decideToFlee();
    int currentFrame = Broodwar->getFrameCount();
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attacks
        return;
    if (currentFrame - _lastAttackFrame == getAttackDuration() + 1)
        clearDamages();
    //if (currentFrame - _lastAttackFrame == Broodwar->getLatency() + 1)
    //    clearDamages();
    if (unit->getGroundWeaponCooldown() <= Broodwar->getLatency() + 1)
    {
        updateRangeEnemies();
        updateTargetEnemy();
        attackEnemyUnit(targetEnemy);
    }
    else if (unit->getGroundWeaponCooldown() <= Broodwar->getLatency())
    {
        ; // do nothing
    }
    else if (unit->getGroundWeaponCooldown() > Broodwar->getLatency() + 2) // == (Broodwar->getLatency()+1)*2
    {
        if(_fleeing)
        {
            //flee();
        }
        else
        {
            //fightMove();
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