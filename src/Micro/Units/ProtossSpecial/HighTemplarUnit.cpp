#include "HighTemplarUnit.h"
#include <UnitsGroup.h>

HighTemplarUnit::HighTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug)
: SpecialUnit(u, ug)
, _lastStormFrame(0)
{
    _mapManager = & MapManager::Instance();
}

HighTemplarUnit::~HighTemplarUnit()
{
}


void HighTemplarUnit::micro()
{
    if (this->unit->getEnergy() >= 75 && Broodwar->getFrameCount() - _lastStormFrame > Broodwar->getLatency() + 3)
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
        if (bestScore > 3 || (_unitsGroup->enemies.size() == 1 && bestScore == 3)) // TOCHANGE bestScore > 1 => > 2 ??
        {
            Broodwar->printf("enemies size : %d", _unitsGroup->enemies.size());
            unit->useTech(BWAPI::TechTypes::Psionic_Storm, bestStormPos);
            _mapManager->stormPos.erase(bestStormPos);
            _lastStormFrame = Broodwar->getFrameCount();
        }
    }
    else if (_fleeing || this->unit->getEnergy() < 74)
    {
        flee();
    }
    else
    {
        //fightMove();
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