#include "DragoonUnit.h"
#include <BWAPI.h>
#include <UnitsGroup.h>

using namespace std;
using namespace BWAPI;

int DragoonUnit::addRange;

std::set<BWAPI::UnitType> DragoonUnit::setPrio;

DragoonUnit::DragoonUnit(BWAPI::Unit* u, UnitsGroup* ug)
:GroundUnit(u, ug)
{
    if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
        addRange = 64;
    else
        addRange = 0;
    _attackDuration += 8; // not static for the moment TODO

    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Terran_Vulture_Spider_Mine);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Lurker);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Observer);
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Dark_Archon);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Carrier);
        setPrio.insert(BWAPI::UnitTypes::Terran_Science_Vessel);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Defiler);
    }
}

DragoonUnit::~DragoonUnit()
{
}

int DragoonUnit::addRangeGround()
{
    return addRange;
}

int DragoonUnit::addRangeAir()
{
    return addRange;
}

void DragoonUnit::micro()
{
    if (targetEnemy != NULL && !(targetEnemy->exists()))
    {
        updateRangeEnemies();
        updateTargetEnemy();
        unit->rightClick(targetEnemy);
        _lastAttackOrder = Broodwar->getFrameCount();
    }
    else
    {
        if (Broodwar->getFrameCount() - _lastAttackOrder > _attackDuration)
        {
            if (unit->getGroundWeaponCooldown() == 0)
            {
                updateRangeEnemies();
                updateTargetEnemy();
                if (targetEnemy) 
                    unit->rightClick(targetEnemy);
                //unit->attackUnit(targetEnemy); // rightClick seems better b/c attackUnit sometimes stuck unit...
                _lastAttackOrder = Broodwar->getFrameCount();
            }
            else if (_fleeing || _lastTotalHP - (unit->getShields() + unit->getHitPoints()) > 0)
            {
                flee();
            }
            else if (!unit->isMoving() && targetEnemy != NULL)
            {
                ///fightMove();
                //switchMode(MODE_INPOS); // TODO
                //updateDir();
                //clickDir();
            }
        }
    }
}

void DragoonUnit::check()
{
    if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
        addRange = 64;
}

bool DragoonUnit::canHit(Unit* enemy)
{
    return enemy->isVisible();
}

bool DragoonUnit::withinRange(Unit* enemy)
{
    return DragoonUnit::getMaxRange() > enemy->getDistance(unit)-enemy->getType().dimensionRight();
}

double DragoonUnit::getMaxRange()
{
    return BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Singularity_Charge) == 1 ? 192.0 : 128.0;
}

int DragoonUnit::getTimeToAttack()
{
    return 8;
}

std::set<BWAPI::UnitType> DragoonUnit::getSetPrio()
{
    return DragoonUnit::setPrio;
}