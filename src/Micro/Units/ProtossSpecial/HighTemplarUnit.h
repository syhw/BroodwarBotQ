#pragma once

#include <SpecialUnit.h>
#include <BWAPI.h>
#include "MapManager.h"

#include <windows.h>
#include <process.h>

class HighTemplarUnit : public SpecialUnit
{
protected:
    MapManager* _mapManager;
    HANDLE _bestStormPosMutex;
    std::pair<Position, int> _bestStormPos;
    std::multimap<double, BWAPI::Unit*> _rangeEnemiesBuf;
    std::map<BWAPI::Unit*, BWAPI::Position> _alliedUnitsPosBuf;
    std::map<BWAPI::Unit*, BWAPI::Position> _enemyUnitsPosBuf;
    std::map<BWAPI::Bullet*, BWAPI::Position> _trackedStormsBuf;
    static DWORD WINAPI StaticLaunchBestStormPos(void* obj);
    DWORD LaunchBestStormPos();
public:
    HighTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~HighTemplarUnit();
    std::pair<BWAPI::Position, int> bestStormPos();
    virtual void micro();
    virtual void check();
    virtual bool canHit(BWAPI::Unit* enemy);
    virtual int getTimeToAttack();
    virtual std::set<BWAPI::UnitType> getSetPrio();
};