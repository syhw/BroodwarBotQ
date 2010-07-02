#include "DragoonUnit.h"
#include <BWAPI.h>
#include <UnitsGroup.h>

using namespace std;
using namespace BWAPI;

DragoonUnit::DragoonUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

DragoonUnit::~DragoonUnit()
{
}

//Need some improvements, but it works.
void DragoonUnit::micro()
{
    std::set<Unit*> enemies;
    int damagesTaken = 0;
    fillEnemies(enemies, damagesTaken);

    std::set<Unit*> enemies_in_range;
    double maxRangeGoon = 0.0;
    double maxRangeGoonEnemy = 0.0;
    fillEnemiesInRangeForDragoon(enemies, enemies_in_range, maxRangeGoon, maxRangeGoonEnemy);

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
    dragoonIA(enemies, maxRangeGoonEnemy);
}

void DragoonUnit::dragoonIA(std::set<Unit*> enemies, double maxRangeGoonEnemy)
{
    for each(Unit* enemy in enemies)
    {
        if (enemy->getOrderTarget() != NULL && (enemy->isStartingAttack() || enemy->isAttacking()) ) 
        {
            Unit* myUnit = enemy->getOrderTarget();
            if (myUnit->getPlayer() == Broodwar->self() && !myUnit->isMoving()) 
            {
                Vec dep(myUnit->getPosition().x() - enemy->getPosition().x(), myUnit->getPosition().y() - enemy->getPosition().y()); 
                dep = dep.normalize();
                Position newPos = Position(myUnit->getPosition().x(), myUnit->getPosition().y());;
                double distanceFromEnemy = dep.norm();
                double calc = (maxRangeGoonEnemy + 30 - enemy->getDistance(newPos))/distanceFromEnemy;
                newPos += Position(static_cast<int>(dep.x * calc), static_cast<int>(dep.y * calc));
                bool test = true;
                for each (Unit* uunit in Broodwar->unitsOnTile(newPos.x(), newPos.y()))
                {
                    if (uunit != myUnit)
                    {
                        test = false;
                        break;
                    }
                    double actualLife = myUnit->getHitPoints() + myUnit->getShields();
                    double totalLife = myUnit->getType().maxHitPoints() + myUnit->getType().maxShields();
                    double probaFuite = 0.2 + 0.8*(1-(distanceFromEnemy/maxRangeGoonEnemy));
                    
                    probaFuite = (actualLife/_unitsGroup->getTotalHP() < 0.1 ? 0 : probaFuite);
                    if (test && (actualLife/totalLife) < probaFuite) 
                    {
                        myUnit->rightClick(newPos);
                        int ux = myUnit->getPosition().x(); int uy = myUnit->getPosition().y();
                        int ex = myUnit->getTargetPosition().x(); int ey = myUnit->getTargetPosition().y();
                        Broodwar->drawLineMap(ux,uy,ex,ey,Colors::Blue);
                        break;
                    }
                }
            }
        }
    }
}

const std::list<BWAPI::UnitType> DragoonUnit::getListePrio() const
{
    return listPriorite;
}