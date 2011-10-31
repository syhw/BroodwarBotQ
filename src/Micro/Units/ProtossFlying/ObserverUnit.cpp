#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossFlying/ObserverUnit.h"
#include "Intelligence/EUnitsFilter.h"
#include "Micro/UnitsGroup.h"

using namespace BWAPI;

ObserverUnit::ObserverUnit(BWAPI::Unit* u,UnitsGroup* ug):FlyingUnit(u, ug)
{
    _eUnitsFilter = & EUnitsFilter::Instance();
}

ObserverUnit::~ObserverUnit()
{
}

void ObserverUnit::micro()
{
    if (Broodwar->getFrameCount() - _lastClickFrame < Broodwar->getLatency())
        return;
    Position invisPos = Positions::None;
    double minDist = 100000.0;
    for (std::map<Unit*, Position>::const_iterator it = _unitsGroup->enemies.begin();
        it != _unitsGroup->enemies.end(); ++it)
    {
        double enemyDist = it->second.getDistance(_unitPos);
        if (_eUnitsFilter->getInvisibleUnits().count(it->first) && enemyDist < minDist)
        {
            minDist = enemyDist;
            invisPos = it->second;
        }
    }
    if (invisPos != Positions::None)
    {
        unit->move(invisPos);
        _lastClickFrame = Broodwar->getFrameCount();
        _lastMoveFrame = Broodwar->getFrameCount();
    } 
    else
        clickTarget();
}

void ObserverUnit::check()
{
}

int ObserverUnit::getAttackDuration()
{
    return 0;
}

std::set<BWAPI::UnitType> ObserverUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}