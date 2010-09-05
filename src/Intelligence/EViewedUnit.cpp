#include "EViewedUnit.h"

using namespace BWAPI;


EViewedUnit::EViewedUnit(Unit* u, unsigned int time)
{
    unit = u;
    update(time);
}

EViewedUnit::EViewedUnit(const EViewedUnit& evu)
: unit(evu.unit)
, type(evu.type)
, HP(evu.HP)
, position(evu.position)
, lastSeen(evu.lastSeen)
, velocity(evu.velocity)
{
}

void EViewedUnit::update(unsigned int time)
{
    type = unit->getType();
    HP = unit->getHitPoints();
    position = unit->getPosition();
    lastSeen = time;
    velocity = Vec(unit->getVelocityX(), unit->getVelocityY());
}