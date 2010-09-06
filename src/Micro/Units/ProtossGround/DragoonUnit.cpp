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
    attackDuration = Broodwar->getLatency() + 8;

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

bool DragoonUnit::decideToFlee()
{
    // TODO complete conditions
    return (_lastTotalHP - (unit->getShields() + unit->getHitPoints()) > 13);
}

void DragoonUnit::micro()
{
    if (targetEnemy != NULL && !(targetEnemy->exists()))
    {
        updateRangeEnemies();
        updateTargetEnemy();
        attackEnemyUnit(targetEnemy);
    }
    else
    {
        if (Broodwar->getFrameCount() - _lastAttackOrder > getAttackDuration())
        {
            if (unit->getGroundWeaponCooldown() == 0)
            {
                updateRangeEnemies();
                updateTargetEnemy();
                attackEnemyUnit(targetEnemy);
            }
            else if (_fleeing || decideToFlee())
            {
                flee();
            }
            else if (!unit->isMoving() && targetEnemy != NULL)
            {
                //fightMove();
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