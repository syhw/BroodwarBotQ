#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Utils/CSingleton.h"
#include "Intelligence/EUnitsFilter.h"
#include <vector>
#include "Utils/Vec.h"

#include <windows.h>
#include <process.h>

/** From BWAPI's doc:
* Positions are measured in pixels and are the highest resolution
* Walk Tiles - each walk tile is an 8x8 square of pixels. These are called walk tiles because walkability data is available at this resolution.
* Build Tiles - each build tile is a 4x4 square of walk tiles, or a 32x32 square of pixels. These are called build tiles because buildability data is available at this resolution. 
*/

// TODO improve storms with units movements (interpolate)
// TODO improve storms by scoring on UnitTypes
// TODO improve the damage map by considering DPS
// TOCHECK perhaps problem when we lift a terran building (we should call onUnitDestroy)
class MapManager: public CSingleton<MapManager>
{
    friend class CSingleton<MapManager>;
    EUnitsFilter* _eUnitsFilter;
    std::map<BWAPI::Unit*, BWAPI::Position> _ourUnits;
    std::map<BWAPI::Unit*, BWAPI::Position> _trackedUnits;
    std::map<BWAPI::Bullet*, BWAPI::Position> _trackedStorms;
    HANDLE _stormPosMutex;
    int _lastStormUpdateFrame;
    std::map<Position, int> _stormPosBuf;
    std::map<Position, int> _dontReStorm;
    std::map<Position, int> _dontReStormBuf;
    std::multimap<double, BWAPI::Unit*> _rangeEnemiesBuf;
    std::map<BWAPI::Unit*, BWAPI::Position> _alliedUnitsPosBuf;
    std::map<BWAPI::Unit*, BWAPI::Position> _enemyUnitsPosBuf;
    std::map<BWAPI::Unit*, std::pair<BWAPI::UnitType, BWAPI::Position> > _invisibleUnitsBuf;
    static DWORD WINAPI StaticLaunchUpdateStormPos(void* obj);
    DWORD LaunchUpdateStormPos();
    inline void updateStormPos();
    int _width;
    int _height;
    int _pix_width;
    int _pix_height;
    MapManager();
    ~MapManager();
    inline void modifyBuildings(BWAPI::Unit* u, bool b);
    inline void addBuilding(BWAPI::Unit* u);
    inline void addAlliedUnit(BWAPI::Unit* u);
    inline void removeBuilding(BWAPI::Unit* u);
    inline void removeAlliedUnit(BWAPI::Unit* u);
    inline void modifyDamages(int* tab, Position p, int minRadius, int maxRadius, int damages);
    inline void updateDamagesGrad(Vec* grad, int* tab, Position p, int minRadius, int maxRadius);
    inline void removeDmg(BWAPI::UnitType ut, BWAPI::Position p);
    inline void removeDmgStorm(BWAPI::Position p);
    inline void addDmg(BWAPI::UnitType ut, BWAPI::Position p);
    inline void addDmgWithoutGrad(BWAPI::UnitType ut, BWAPI::Position p);
    inline void addDmgStorm(BWAPI::Position p);
    inline int additionalRangeGround(BWAPI::UnitType ut);
    inline int additionalRangeAir(BWAPI::UnitType ut);

public:
    bool* walkability;          // walk tiles
    bool* lowResWalkability;    // low res => building tiles
    bool* buildings_wt;         // walk tiles
    bool* buildings_wt_strict;  // walk tiles
    bool* buildings;            // low res => building tiless
    int* groundDamages;         // build tiles
    int* airDamages;            // build tiles
    Vec* groundDamagesGrad;     // build tiles
    Vec* airDamagesGrad;        // build tiles
	std::map<BWTA::Region*, BWAPI::Position> regionsPFCenters; // Pathfinding wise region centers
	std::map<BWTA::Region*, std::map<BWTA::Region*, double> > distRegions; // distRegions[R1][R2] w.r.t regionsPFCenters
	std::map<BWTA::Region*, BWAPI::TilePosition> regionsInsideCenter; // Centers of the regions that are inside
    std::map<Position, int> stormPos;
    void onUnitCreate(BWAPI::Unit* u);
    void onUnitDestroy(BWAPI::Unit* u);
    void onUnitShow(BWAPI::Unit* u);
    void onUnitHide(BWAPI::Unit* u);
    void justStormed(BWAPI::Position p);
    void update();
    const std::map<BWAPI::Unit*, BWAPI::Position> & getOurUnits();
    const std::map<BWAPI::Unit*, BWAPI::Position> & getTrackedUnits();
    const std::map<BWAPI::Bullet*, BWAPI::Position> & getTrackedStorms();
    BWAPI::Position closestWalkabableSameRegionOrConnected(BWAPI::Position p);
    BWAPI::TilePosition closestWalkabableSameRegionOrConnected(BWAPI::TilePosition tp);
	bool isBTWalkable(int x, int y);
	bool isBTWalkable(const BWAPI::TilePosition& tp);
    void drawBuildings();           // debug
    void drawBuildingsStrict();     // debug
    void drawWalkability();         // debug
    void drawLowResWalkability();   // debug
    void drawLowResBuildings();     // debug
    void drawGroundDamages();       // debug
    void drawAirDamages();          // debug
    void drawGroundDamagesGrad();   // debug
    void drawAirDamagesGrad();      // debug
    void drawBestStorms();          // debug
};
