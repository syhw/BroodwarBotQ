#include "DragoonUnit.h"
#include <BWAPI.h>
#include <UnitsGroup.h>

using namespace std;
using namespace BWAPI;

int DragoonUnit::addRange;

BWAPI::UnitType DragoonUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

DragoonUnit::DragoonUnit(BWAPI::Unit* u, UnitsGroup* ug)
:GroundUnit(u, ug)
{
    if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
        addRange = 64;
    else
        addRange = 0;
    _attackDuration += 8; // not static for the moment TODO
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
    if (_unitsGroup->enemies.empty())
    {
        if (_unitsGroup->size() < 23)
            this->switchMode(MODE_FLOCK);
        else 
            this->switchMode(MODE_MOVE);
        return;
    }
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
                unit->rightClick(targetEnemy);
                //unit->attackUnit(targetEnemy); // rightClick seems better b/c attackUnit sometimes stuck unit...
                _lastAttackOrder = Broodwar->getFrameCount();
            }
            else if (_fleeing || _lastTotalHP - (unit->getShields() + unit->getHitPoints()) > 0)
            {
                ///flee();
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

BWAPI::UnitType* DragoonUnit::getListPriorite()
{
    return DragoonUnit::listPriorite;
}