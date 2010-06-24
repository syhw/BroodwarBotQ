#include "EUnitsFilter.h"
#include "Data.inl"

using namespace BWAPI;
using namespace std;

EUnitsFilter::EUnitsFilter()
: BaseObject(std::string("EUnitsFilter"))
, eViewedUnits(std::map<Unit*, EViewedUnit>(), "EViewedUnits", this)
{
    timeManager = & TimeManager::Instance();
}

void EUnitsFilter::update(Unit* u)
{
    sout << "plop, j update" << sendl;
    if (u->getPlayer() == Broodwar->self()) return;
    if (u->getPlayer()->isNeutral()) return;
    Broodwar->printf("eViedUnits.size() : %d", eViewedUnits->size());
    if (eViewedUnits->count(u))
        (*eViewedUnits)[u].update(timeManager->getElapsedTime());
    else 
        (*eViewedUnits)[u] = EViewedUnit(u, timeManager->getElapsedTime());
        //eViewedUnits.insert(make_pair(u, EViewedUnit(u, timeManager->getElapsedTime())));
#ifdef USE_MONITOR
    // callback to Qt to modify EUFilter tab's QTableWidget with current values
#endif
    serr << "!!!!!, j update" << sendl;
}

void EUnitsFilter::onUnitDestroy(Unit* u)
{
    if (u->getPlayer() == Broodwar->self()) return;
    if (u->getPlayer()->isNeutral()) return;
    eViewedUnits->erase(u);
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

