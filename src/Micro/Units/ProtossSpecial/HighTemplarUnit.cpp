#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossSpecial/HighTemplarUnit.h"
#include "Micro/UnitsGroup.h"
#include "Regions/MapManager.h"

int HighTemplarUnit::lastStormableUnitsUpdateFrame;

HighTemplarUnit::HighTemplarUnit(BWAPI::Unit* u)
: SpecialUnit(u)
, _lastStormFrame(0)
, _mergingFrame(0)
, _bestStormPos(Positions::None)
, _bestFartherStormPos(Positions::None)
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
	/// Lag code
	if (Broodwar->getFrameCount() - _lastClickFrame <= Broodwar->getLatencyFrames())
		return;

    int elapsed = Broodwar->getFrameCount() - _lastStormFrame;
	/// Do not interupt a storm being cast
    if (elapsed <= Broodwar->getLatencyFrames() + getAttackDuration())
        return;

	/// Merge if that's the most interesting thing to do
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
    
    if (_fleeing)
        flee();
    else
    {
        //fightMove();
		// We don't want our templars far from the units group center
		if (unit->getEnergy() > 72)
		{
			if (_bestFartherStormPos != Positions::None
				&& _bestFartherStormPos.getApproxDistance(_unitPos) >= 9*TILE_SIZE)
				this->move(_bestFartherStormPos);
			else if (_bestStormPos != Positions::None
				&& _bestStormPos.getApproxDistance(_unitPos) >= 9*TILE_SIZE)
				this->move(_bestStormPos);
			else
				this->move(_unitsGroup->center);
		}
		else
			this->move(_unitsGroup->center);
		_lastClickFrame = Broodwar->getFrameCount();
		_lastMoveFrame = Broodwar->getFrameCount();
    }
	// Try and storm if it has any advantage
	if (this->unit->getEnergy() > 74 && elapsed > Broodwar->getLatencyFrames() + 42) // TODO magic number 42
    {   
        int bestScore = -1000;
		int bestFartherScore = -1000;
        if (elapsed > 128)
        {
			// Search the best possible storm
            for (std::map<Position, int>::const_iterator it = _mapManager->stormPos.begin();
                it != _mapManager->stormPos.end(); ++it)
            {
				if (it->second > bestFartherScore && _unitPos.getApproxDistance(it->first) < 512.0)
				{
					bestFartherScore = it->second;
					_bestFartherStormPos = it->first;
				}
                if (it->second > bestScore && _unitPos.getApproxDistance(it->first) < 288.0 && _lastStormPos.getDistance(it->first) > 46)
                {
                    bestScore = it->second;
                    _bestStormPos = it->first;
                }
            }
        } 
        else
        {
			// Search in reverse
            for (std::map<Position, int>::const_reverse_iterator it = _mapManager->stormPos.rbegin();
                it != _mapManager->stormPos.rend(); ++it)
            {
				if (it->second > bestFartherScore && _unitPos.getApproxDistance(it->first) < 512.)
				{
					bestFartherScore = it->second;
					_bestFartherStormPos = it->first;
				}
                if (it->second > bestScore && _unitPos.getApproxDistance(it->first) < 288.0 && _lastStormPos.getDistance(it->first) > 46)
                {
                    bestScore = it->second;
                    _bestStormPos = it->first;
                }
            }
        }
        // Storm only if it damages at least 2 units, or at least 1 invisible unit,
        // or there is only one enemy unit around us and we can storm it without collateral damages
        if (bestScore > 4 || (_unitsGroup->enemies.size() == 1 && bestScore == 3))
        {       
            unit->useTech(BWAPI::TechTypes::Psionic_Storm, _bestStormPos);
            //Broodwar->printf("Frame %d, pos (%d, %d), stormPos size %d", Broodwar->getFrameCount(), bestStormPos.x(), bestStormPos.y(), _mapManager->stormPos.size());
            // tell the MapManager that we just stormed here
            _mapManager->justStormed(_bestStormPos);
            _lastStormFrame = Broodwar->getFrameCount();
            _lastStormPos = _bestStormPos;
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