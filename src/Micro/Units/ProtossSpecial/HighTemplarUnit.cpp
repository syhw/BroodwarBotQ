#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossSpecial/HighTemplarUnit.h"
#include "Micro/UnitsGroup.h"

int HighTemplarUnit::lastStormableUnitsUpdateFrame;

HighTemplarUnit::HighTemplarUnit(BWAPI::Unit* u,UnitsGroup* ug)
: SpecialUnit(u, ug)
, _lastStormFrame(0)
, _mergingFrame(0)
{
    _mapManager = & MapManager::Instance();
}

HighTemplarUnit::~HighTemplarUnit()
{
}

void HighTemplarUnit::micro()
{
    if (_mergingFrame != 0)
    {
        if (Broodwar->getFrameCount() - _mergingFrame < 80)
            return;
    }
    int elapsed = Broodwar->getFrameCount() - _lastStormFrame;
    if (elapsed <= Broodwar->getLatencyFrames() + getAttackDuration())
        return;
    if (((!Broodwar->self()->hasResearched(BWAPI::TechTypes::Psionic_Storm) && unit->getEnergy() < 75) || unit->getEnergy() < 20 /* TODO */ || (unit->getEnergy() < 74 && unit->getHitPoints() < 20) 
        || (unit->getEnergy() < 55 && unit->getShields() < 2)) && elapsed > Broodwar->getLatencyFrames() + getAttackDuration())
    {
        _unitsGroup->signalMerge(unit);
        _mergingFrame = Broodwar->getFrameCount();
        return;
    }

    if (dodgeStorm())
        return;

    // Updating the map of stormable units
    if (lastStormableUnitsUpdateFrame != Broodwar->getFrameCount())
    {
        stormableUnits.clear();
        lastStormableUnitsUpdateFrame = Broodwar->getFrameCount();
    }
    for (std::map<Unit*, Position>::const_iterator it = _unitsGroup->enemies.begin();
        it != _unitsGroup->enemies.end(); ++it)
    {
        if (it->first->isVisible() && it->first->getType().isBuilding())
            continue;
        if (!stormableUnits.count(it->first))
            stormableUnits.insert(*it);
    }

    // Try and storm if it has any advantage, otherwise flee or don't stuck
    if (this->unit->getEnergy() > 75 && elapsed > Broodwar->getLatencyFrames() + getAttackDuration())
    {   
        Position bestStormPos;
        int bestScore = -1;
        if (elapsed > 128)
        {
            for (std::map<Position, int>::const_iterator it = _mapManager->stormPos.begin();
                it != _mapManager->stormPos.end(); ++it)
            {
                if (it->second > bestScore && _unitPos.getDistance(it->first) < 288.0 && (elapsed > 64 || _lastStormPos.getDistance(it->first) > 46))
                {
                    bestScore = it->second;
                    bestStormPos = it->first;
                }
            }
        } 
        else
        {
            for (std::map<Position, int>::const_reverse_iterator it = _mapManager->stormPos.rbegin();
                it != _mapManager->stormPos.rend(); ++it)
            {
                if (it->second > bestScore && _unitPos.getDistance(it->first) < 288.0 && (elapsed > 64 || _lastStormPos.getDistance(it->first) > 46))
                {
                    bestScore = it->second;
                    bestStormPos = it->first;
                }
            }
        }
        // Storm only if it damages at least 2 units, or at least 1 invisible unit,
        // or there is only one enemy unit around us and we can storm it without collateral damages
        if (bestScore > 3 || (_unitsGroup->enemies.size() == 1 && bestScore == 3))
        {       
            unit->useTech(BWAPI::TechTypes::Psionic_Storm, bestStormPos);
            //Broodwar->printf("Frame %d, pos (%d, %d), stormPos size %d", Broodwar->getFrameCount(), bestStormPos.x(), bestStormPos.y(), _mapManager->stormPos.size());
            // tell the MapManager that we just stormed here
            _mapManager->justStormed(bestStormPos);
            _lastStormFrame = Broodwar->getFrameCount();
            _lastStormPos = bestStormPos;
        }
    }
    else if (_fleeing)
    {
        flee();
    }
    else
    {
        //fightMove();
        if (Broodwar->getFrameCount() - _lastClickFrame <= Broodwar->getLatencyFrames())
            return;
        if (_unitsGroup->distToNearestChoke < 128.0 && _unitsGroup->enemiesAltitude > _unitsGroup->groupAltitude)
        {
            clickTarget();
            _fightMoving = true;
        }
        else if (target == _unitPos)
        {
            this->move(_unitsGroup->center);
        }
        else
        {
            if (_unitPos.getDistance(target) > 192.0)
                this->move(target);
        }
    }
}

void HighTemplarUnit::check()
{
}

std::set<BWAPI::UnitType> HighTemplarUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}