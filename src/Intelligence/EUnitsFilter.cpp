#include "EUnitsFilter.h"

using namespace BWAPI;
using namespace std;

void EUnitsFilter::update(Unit* u)
{
    if (u->getPlayer() == Broodwar->self()) return;
    if (u->getPlayer()->isNeutral()) return;
 //   Broodwar->printf("eViewedUnits.size() : %d", eViewedUnits.size());
    if (eViewedUnits.count(u))
        eViewedUnits[u].update(timeManager->getElapsedTime());
    else 
        eViewedUnits[u] = EViewedUnit(u, timeManager->getElapsedTime());
        //eViewedUnits.insert(make_pair(u, EViewedUnit(u, timeManager->getElapsedTime())));
#ifdef USE_MONITOR
    // callback to Qt to modify EUFilter tab's QTableWidget with current values
#endif
}

EUnitsFilter::EUnitsFilter()
{
    timeManager = & TimeManager::Instance();
}

void EUnitsFilter::onUnitDestroy(Unit* u)
{
    if (u->getPlayer() == Broodwar->self()) return;
    if (u->getPlayer()->isNeutral()) return;
    eViewedUnits.erase(u);
}

void EUnitsFilter::onUnitMorph(Unit* u)
{
    update(u);
}

void EUnitsFilter::onUnitShow(Unit* u)
{
    update(u);
}

void EUnitsFilter::onUnitHide(Unit* u)
{
    update(u);
}

void EUnitsFilter::onUnitRenegade(Unit* u)
{
    update(u);
}