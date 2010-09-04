#include "HighTemplarUnit.h"
#include <UnitsGroup.h>

HighTemplarUnit::HighTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug)
: SpecialUnit(u, ug)
, _bestStormPosMutex(CreateMutex( 
        NULL,                  // default security attributes
        FALSE,                 // initially not owned
        NULL)                  // unnamed mutex
)
, _bestStormPos(std::pair<Position, int>(Position(0, 0), -10000))
{
    _mapManager = & MapManager::Instance();
}

HighTemplarUnit::~HighTemplarUnit()
{
}

DWORD WINAPI HighTemplarUnit::StaticLaunchBestStormPos(void* obj)
{
    HighTemplarUnit* This = (HighTemplarUnit*) obj;
    int buf = This->LaunchBestStormPos();
    ExitThread(buf);
    return buf;
}

DWORD HighTemplarUnit::LaunchBestStormPos()
{
    DWORD waitResult = WaitForSingleObject(
        _bestStormPosMutex,
        100);

    switch (waitResult) 
    {
    case WAIT_OBJECT_0: 
        _bestStormPos = bestStormPos();
        ReleaseMutex(_bestStormPosMutex);
        break; 

    case WAIT_ABANDONED:
        ReleaseMutex(_bestStormPosMutex);
        return -1;
    }
    return 0;
}

std::pair<BWAPI::Position, int> HighTemplarUnit::bestStormPos()
{
    std::set<Position> stormableUnits;
    std::set<Position> possiblePos;
    if (_rangeEnemiesBuf.empty())
        Broodwar->printf("Range Enemies EMPTY");
    for (std::multimap<double, BWAPI::Unit*>::const_iterator it = _rangeEnemiesBuf.begin();
        it != _rangeEnemiesBuf.end(); ++it)
    {
        if (it->first < 415.0) // TOCHANGE 415, sup approx of 9(storm range)x46(diag tile)
            stormableUnits.insert(_enemyUnitsPosBuf[it->second]);
    }
    for (std::set<Position>::const_iterator it = stormableUnits.begin();
        it != stormableUnits.end(); ++it)
    {
        Position tmpPos = (*it);
        if (stormableUnits.size() > 16) // TOCHANGE 16 units
            possiblePos.insert(tmpPos);
        else
        {
            possiblePos.insert(tmpPos);
            possiblePos.insert(Position(tmpPos.x() - 32, tmpPos.y()));
            possiblePos.insert(Position(tmpPos.x() + 32, tmpPos.y()));
            possiblePos.insert(Position(tmpPos.x(), tmpPos.y() - 32));
            possiblePos.insert(Position(tmpPos.x(), tmpPos.y() + 32));
            possiblePos.insert(Position(tmpPos.x() - 32, tmpPos.y() - 32));
            possiblePos.insert(Position(tmpPos.x() + 32, tmpPos.y() + 32));
            possiblePos.insert(Position(tmpPos.x() + 32, tmpPos.y() - 32));
            possiblePos.insert(Position(tmpPos.x() - 32, tmpPos.y() + 32));
        }
    }
    Position bestPos;
    int max = -1000000;
    //if (possiblePos.empty())
        //Broodwar->printf("Possible Pos EMPTY");
    //if (_alliedUnitsPosBuf.empty())
        //Broodwar->printf("allied Pos EMPTY");
    //if (_enemyUnitsPosBuf.empty())
        //Broodwar->printf("enemy Pos EMPTY");
    for (std::set<Position>::const_iterator it = possiblePos.begin();
        it != possiblePos.end(); ++it)
    {
        int tmp = 0;
        for (std::map<BWAPI::Unit*, BWAPI::Position>::const_iterator uit = _alliedUnitsPosBuf.begin();
            uit != _alliedUnitsPosBuf.end(); ++uit)
        {
            //if ((*uit)->unit->getDistance(*it) < 32.0+16.0+5.0) // TO CHANGE TOCHANGE 5.0 to account for the diag
            //    --tmp;
            Vec dist(it->x() - uit->second.x(), it->y() - uit->second.y());
            if (dist.norm() < 32.0 + 16.0 + 5.0)
                tmp -= 2;
        }
        for (std::map<BWAPI::Unit*, BWAPI::Position>::const_iterator eit = _enemyUnitsPosBuf.begin();
            eit != _enemyUnitsPosBuf.end(); ++eit)
        {
            Vec dist(it->x() - eit->second.x(), it->y() - eit->second.y());
            if (dist.norm() < 32.0 + 16.0 + 5.0)
                ++tmp;
            //if ((*eit)->getDistance(*it) < 32.0+16.0+5.0) // TO CHANGE TOCHANGE 5.0 to account for the diag
            //    ++tmp;
            //if (!((*eit)->isVisible())) // Lurkers and other sneakers
            //    ++tmp;
        }
        if (tmp > max)
        {
            max = tmp;
            bestPos = *it;
        }
    }
    Broodwar->printf("best storm pos: (%d, %d)", bestPos.x(), bestPos.y());
    return std::pair<Position, int>(bestPos, max);
}

void HighTemplarUnit::micro()
{
    if (this->unit->getEnergy() >= 75)
    {
        if (WaitForSingleObject(_bestStormPosMutex, 0) == WAIT_OBJECT_0) // cannot enter when the thread is running
        {
            Broodwar->drawBoxMap(_bestStormPos.first.x() - 48, _bestStormPos.first.x() + 48,
                _bestStormPos.first.y() - 48, _bestStormPos.first.y() + 48, BWAPI::Colors::Purple);
            if (_bestStormPos.second > 0)
            {
                Broodwar->printf("firing a storm");
                unit->useTech(BWAPI::TechTypes::Psionic_Storm, _bestStormPos.first);
            }
            else
            {
                //Broodwar->printf("Creating a thread");
                _rangeEnemiesBuf = _rangeEnemies;
                _alliedUnitsPosBuf = _mapManager->getOurUnits();
                _enemyUnitsPosBuf = _mapManager->getTrackedUnits();
                _trackedStormsBuf = _mapManager->getTrackedStorms();
                _bestStormPos = bestStormPos();
                /*DWORD threadId;
                HANDLE thread = CreateThread( 
                    NULL,                   // default security attributes
                    0,                      // use default stack size  
                    &HighTemplarUnit::StaticLaunchBestStormPos,      // thread function name
                    (void*) this,                   // argument to thread function 
                    0,                      // use default creation flags 
                    &threadId);             // returns the thread identifier 
                CloseHandle(thread);*/
            }
        }
        ReleaseMutex(_bestStormPosMutex);
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