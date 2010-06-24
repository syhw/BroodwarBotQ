#include "EViewedUnit.h"

using namespace BWAPI;


EViewedUnit::EViewedUnit(Unit* u, unsigned long time)
{
    unit = u;
    update(time);
}

void EViewedUnit::update(unsigned long time)
{
    HP = unit->getHitPoints();
    position = unit->getPosition();
    kills = unit->getKillCount();
    lastSeen = time;
    velocity = Vec(unit->getVelocityX(), unit->getVelocityY());
}