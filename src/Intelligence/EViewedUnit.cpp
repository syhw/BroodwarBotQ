#include "EViewedUnit.h"

using namespace BWAPI;


EViewedUnit::EViewedUnit(Unit* u, unsigned long time)
{
    unit = u;
    update(time);
}

EViewedUnit::EViewedUnit(const EViewedUnit& evu)
: unit(evu.unit)
, HP(evu.HP)
, position(evu.position)
, kills(evu.kills)
, lastSeen(evu.lastSeen)
, velocity(evu.velocity)
{
}

void EViewedUnit::update(unsigned long time)
{
    HP = unit->getHitPoints();
    position = unit->getPosition();
    kills = unit->getKillCount();
    lastSeen = time;
    velocity = Vec(unit->getVelocityX(), unit->getVelocityY());
}

/*std::ostream& EViewedUnit::operator <<(std::ostream& os) const
{
    os << "unit: " << (unsigned long)unit
        << ", HP: " << HP // todo position
        << ", kills: " << kills
        << ", lastSeen:" << lastSeen
        << ", velocity:" << velocity;
    return os;
}*/

std::ostream& operator <<(std::ostream& os, const EViewedUnit& evu)
{
    os << "unit: " << (unsigned long)evu.unit
        << ", HP: " << evu.HP // todo position
        << ", kills: " << evu.kills
        << ", lastSeen:" << evu.lastSeen
        << ", velocity:" << evu.velocity;
    return os;
}

const std::istream& EViewedUnit::operator >>(const std::istream& is) const
{
// todo !
    return is;
}
