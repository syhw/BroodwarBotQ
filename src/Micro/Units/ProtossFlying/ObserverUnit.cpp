#include "ObserverUnit.h"
#include "EUnitsFilter.h"
#include "UnitsGroup.h"

using namespace BWAPI;

ObserverUnit::ObserverUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
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
    int minDist = 100000;
    for (std::map<Unit*, Position>::const_iterator it = _unitsGroup->enemies.begin();
        it != _unitsGroup->enemies.end(); ++it)
    {
        int enemyDist = it->second.getDistance(_unitPos);
        if (_eUnitsFilter->getInvisibleUnits().count(it->first) && enemyDist < minDist)
        {
            minDist = enemyDist;
            invisPos = it->second;
        }
    }
    if (invisPos != Positions::None)
    {
        Broodwar->printf("HERE");
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