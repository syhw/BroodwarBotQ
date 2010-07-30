#include "EUnit.h"

EUnit::EUnit(BWAPI::Unit *self, double distance):m_self(self), m_distance(distance), m_damageTaken(0), m_hp(self->getHitPoints()+self->getShields())
{
    
}

double& EUnit::distance()
{
    return m_distance;
}

void EUnit::update()
{
    m_hp = m_self->getHitPoints()+m_self->getShields();
    m_damageTaken = 0;
}

int& EUnit::damageTaken()
{
    return m_damageTaken;
}

BWAPI::Unit* EUnit::self()
{
    return m_self;
}

int EUnit::getPrio(pBayesianUnit u)
{
    unsigned int val(0);
    if (m_damageTaken > m_hp)
        return -1;

    double diffDistance = max(u->unit->getDistance(m_self) - m_distance, 0);

    if (diffDistance)
        val += static_cast<int>(diffDistance);

    int hpAfter = m_hp - m_damageTaken;
    val += hpAfter/u->damagesOn(m_self);
    
    BWAPI::UnitType* tab = u->getListPriorite();
    for (unsigned int i = 0; i < NUMBER_OF_PRIORITY; i++)
        if (tab[i] == m_self->getType())
            val += i;

    return val;
}