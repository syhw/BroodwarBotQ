#include "ZealotUnit.h"
#include <BWAPI.h>
#include <UnitsGroup.h>

using namespace std;
using namespace BWAPI;

ZealotUnit::ZealotUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

ZealotUnit::~ZealotUnit()
{
}

void ZealotUnit::micro()
{
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
        else
            Broodwar->drawLineMap(unit->getPosition().x(),      unit->getPosition().y(),
                                  unit->getTargetPosition().x(),unit->getTargetPosition().y(),
                                  Colors::White);
    }
    enemies_in_range.clear();
}

const std::list<BWAPI::UnitType> ZealotUnit::getListePrio() const
{
    return listPriorite;
}