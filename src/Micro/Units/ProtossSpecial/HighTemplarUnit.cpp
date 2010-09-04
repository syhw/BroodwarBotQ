#include "HighTemplarUnit.h"
#include <UnitsGroup.h>

HighTemplarUnit::HighTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug)
: SpecialUnit(u, ug)
{
    _mapManager = & MapManager::Instance();
}

HighTemplarUnit::~HighTemplarUnit()
{
}


void HighTemplarUnit::micro()
{
    if (this->unit->getEnergy() >= 75)
    {   
        Position bestStormPos;
        int bestScore = -1;
        for (std::map<Position, int>::const_iterator it = _mapManager->stormPos.begin();
            it != _mapManager->stormPos.end(); ++it)
        {
            if (it->second > bestScore && _unitPos.getDistance(it->first) < 415.0)
            {
                bestScore = it->second;
                bestStormPos = it->first;
            }
        }
        if (bestScore > 0)
        {
            unit->useTech(BWAPI::TechTypes::Psionic_Storm, bestStormPos);
            _mapManager->stormPos.erase(bestStormPos);
        }
    }
    else if (_fleeing || this->unit->getEnergy() < 74)
    {
        flee();
    }
    else
    {
        fightMove();
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

std::set<BWAPI::UnitType> HighTemplarUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}