#include "HighTemplarUnit.h"
#include <UnitsGroup.h>

BWAPI::UnitType HighTemplarUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

HighTemplarUnit::HighTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug):SpecialUnit(u, ug)
{
}

HighTemplarUnit::~HighTemplarUnit()
{
}

void HighTemplarUnit::micro()
{
    if (_unitsGroup->enemies.empty())
    {
        if (_unitsGroup->size() < 23)
            this->switchMode(MODE_FLOCK);
        else 
            this->switchMode(MODE_MOVE);
        return;
    }
    if (this->unit->getEnergy() >= 75)
    {
        updateRangeEnemies();
        updateTargetEnemy();
        unit->useTech(BWAPI::TechTypes::Psionic_Storm, targetEnemy->getPosition());
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

void HighTemplarUnit::check()
{
}

bool HighTemplarUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("HighTemplarUnit::canHit non implémenté !");
#endif
    return false;
}

int HighTemplarUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("HighTemplarUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

BWAPI::UnitType* HighTemplarUnit::getListPriorite()
{
    return HighTemplarUnit::listPriorite;
}