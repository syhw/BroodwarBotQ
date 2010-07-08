#include "ZealotUnit.h"
#include <BWAPI.h>
#include <UnitsGroup.h>

using namespace std;
using namespace BWAPI;

BWAPI::UnitType ZealotUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Probe};

ZealotUnit::ZealotUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

ZealotUnit::~ZealotUnit()
{
}

void ZealotUnit::micro()
{
   /* 
    std::set<Unit*> enemies;
    std::set<Unit*> enemies_in_range;
    int damagesTaken = 0;

    fillEnemies(enemies, damagesTaken);

    double maxRangeZealot = unit->getType().groundWeapon().maxRange();
    double maxRangeZealotEnemy = 0.0;
    for each(Unit* enemy in enemies)
    {
        if (maxRangeZealotEnemy == 0.0)
            maxRangeZealotEnemy = enemy->getType().groundWeapon().maxRange();

        if (unit->getDistance(enemy) < maxRangeZealot) 
            enemies_in_range.insert(enemy);
    }

    Unit* weakestenemy = UnitsGroup::findWeakestEnemy(enemies_in_range);

    if (weakestenemy)
        attackEnemy(weakestenemy, Colors::Red);
    else
    {
        Unit* closest_enemy = findClosestEnemy(enemies);
        if (closest_enemy)
            attackEnemy(closest_enemy, Colors::Yellow);
#ifdef __DEBUG_NICOLAS__
        else
            BWAPI::Broodwar->drawLineMap(unit->getPosition().x(),      unit->getPosition().y(),
                                  unit->getTargetPosition().x(),unit->getTargetPosition().y(),
                                  Colors::White);
#endif
    }
    enemies_in_range.clear();
    */
}

bool ZealotUnit::canHit(BWAPI::Unit* enemy)
{
    return enemy->isVisible() && !enemy->getType().isFlyer() && (enemy->getDistance(unit) > 0);
}

int ZealotUnit::getTimeToAttack()
{
    BWAPI::Broodwar->printf("ZealotUnit::getTimeToAttack non implémenté !");
    return 0;
}

BWAPI::UnitType* ZealotUnit::getListPriorite()
{
    return ZealotUnit::listPriorite;
}