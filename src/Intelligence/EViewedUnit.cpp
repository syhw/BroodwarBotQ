#include <PrecompiledHeader.h>
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
    lastSeen = time;
    if (unit->getPosition() == BWAPI::Positions::Unknown || unit->getPosition() == BWAPI::Positions::None || unit->getPosition() == BWAPI::Positions::Invalid)
        return;
    type = unit->getType();
    HP = unit->getHitPoints();
    position = unit->getPosition();
    velocity = Vec(unit->getVelocityX(), unit->getVelocityY());
}