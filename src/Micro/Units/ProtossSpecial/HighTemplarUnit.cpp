#include "HighTemplarUnit.h"
#include <UnitsGroup.h>

int HighTemplarUnit::lastStormableUnitsUpdateFrame;

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
    // Updating the map of stormable units
    if (lastStormableUnitsUpdateFrame != Broodwar->getFrameCount())
    {
        stormableUnits.clear();
        lastStormableUnitsUpdateFrame = Broodwar->getFrameCount();
    }
    for (std::set<Unit*>::const_iterator it = _unitsGroup->enemies.begin();
        it != _unitsGroup->enemies.end(); ++it)
    {
        if ((*it)->getType().isBuilding())
            continue;
        if (!stormableUnits.count(*it))
            stormableUnits.insert(std::make_pair<Unit*, Position>(*it, (*it)->getPosition()));
    }

    // Try and storm if it has any advantage, otherwise flee or don't stuck
    if (this->unit->getEnergy() >= 75 && Broodwar->getFrameCount() - _lastStormFrame > Broodwar->getLatency() + getAttackDuration())
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
        // Storm only if it damages at least 2 units, or at least 1 invisible unit,
        // or there is only one enemy unit around us and we can storm it without collateral damages
        if (bestScore > 3 || (_unitsGroup->enemies.size() == 1 && bestScore == 3))
        {
            //Broodwar->printf("enemies size : %d", _unitsGroup->enemies.size());
            unit->useTech(BWAPI::TechTypes::Psionic_Storm, bestStormPos);
            // remove the storm that we just fired
            _mapManager->stormPos.erase(bestStormPos);
            // remove the >= 4/9 (buildtiles) overlaping storms yet present in stormPos
            for (std::map<Position, int>::iterator it = _mapManager->stormPos.begin();
                it != _mapManager->stormPos.end(); )
            {
                if (it->first.getDistance(bestStormPos) < 46.0)
                {
                    std::map<Position, int>::iterator tmp = it;
                    ++it;
                    _mapManager->stormPos.erase(tmp);
                }
                else
                    ++it;
            }
            _lastStormFrame = Broodwar->getFrameCount();
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

std::set<BWAPI::UnitType> HighTemplarUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}