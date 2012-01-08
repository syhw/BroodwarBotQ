#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Utils/CSingleton.h"
#include "Intelligence/EUnitsFilter.h"
#include <vector>
#include "Utils/Vec.h"

#include <windows.h>
#include <process.h>

#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/serialization/map.hpp"
#include "boost/serialization/utility.hpp"

class BayesianUnit;

struct PathfindWork
{
	BayesianUnit* bunit;
	BWAPI::Unit* unit;
	BWAPI::TilePosition start;
	BWAPI::TilePosition end;
	int damages;
	bool flyer;
	std::vector<BWAPI::TilePosition> btpath;
	PathfindWork(BayesianUnit* ptr, BWAPI::Unit* u, BWAPI::TilePosition s, BWAPI::TilePosition e, int dmg, bool f = false)
		: bunit(ptr)
		, unit(u)
		, start(s)
		, end(e)
		, damages(dmg)
		, flyer(f)
	{
	}
};

struct PathAwareMaps
{
    friend class boost::serialization::access;
	template <class archive>
    void serialize(archive & ar, const unsigned int version)
    {
        ar & regionsPFCenters;
		ar & distRegions;
        ar & distBaseToBase;
    }
	std::map<int, std::pair<int, int> > regionsPFCenters; // Pathfinding wise region centers
	std::map<int, std::map<int, double> > distRegions; // distRegions[R1][R2] w.r.t regionsPFCenters
	std::map<int, std::map<int, double> > distBaseToBase;
};

BOOST_CLASS_TRACKING(PathAwareMaps, boost::serialization::track_never);
BOOST_CLASS_VERSION(PathAwareMaps, 1);

/** From BWAPI's doc:
* Positions are measured in pixels and are the highest resolution
* Walk Tiles - each walk tile is an 8x8 square of pixels. These are called walk tiles because walkability data is available at this resolution.
* Build Tiles - each build tile is a 4x4 square of walk tiles, or a 32x32 square of pixels. These are called build tiles because buildability data is available at this resolution. 
*/
// #define __BUILDINGS_WT_STRICT__

// TODO improve storms with units movements (interpolate)
// TODO improve storms by scoring on UnitTypes
// TODO improve the damage map by considering DPS
// TOCHECK perhaps problem when we lift a terran building (we should call onUnitDestroy)
class MapManager : public CSingleton<MapManager>
{
    friend class CSingleton<MapManager>;
    EUnitsFilter* _eUnitsFilter;
    std::map<BWAPI::Unit*, BWAPI::Position> _ourUnits;
    std::map<BWAPI::Unit*, BWAPI::Position> _trackedUnits;
    std::map<BWAPI::Bullet*, BWAPI::Position> _trackedStorms;
    HANDLE _stormPosMutex;
    HANDLE _signalLaunchStormUpdate;
	HANDLE _stormThread;
    static unsigned __stdcall StaticLaunchUpdateStormPos(void* obj);
    DWORD LaunchUpdateStormPos();
    HANDLE _pathfindingMutex;
    HANDLE _signalLaunchPathfinding;
    HANDLE _pathfindingThread;
    static unsigned __stdcall StaticLaunchPathfinding(void* obj);
    DWORD LaunchPathfinding();
    int _lastStormUpdateFrame;
    std::map<Position, int> _stormPosBuf;
    std::map<Position, int> _dontReStorm;
    std::map<Position, int> _dontReStormBuf;
    std::multimap<double, BWAPI::Unit*> _rangeEnemiesBuf;
    std::map<BWAPI::Unit*, BWAPI::Position> _alliedUnitsPosBuf;
    std::map<BWAPI::Unit*, BWAPI::Position> _enemyUnitsPosBuf;
    std::map<BWAPI::Unit*, std::pair<BWAPI::UnitType, BWAPI::Position> > _invisibleUnitsBuf;
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

	inline void registerPathfindWork(BayesianUnit* ptr, BWAPI::Unit* u, BWAPI::TilePosition start, BWAPI::TilePosition end, int damages);
	std::list<PathfindWork> _pathfindWorks;
	PathfindWork _currentPathfindWork;
	bool _currentPathfindWorkAborded;
    bool* _buildingsBuf;            // low res => building tiless
    int* _groundDamagesBuf;         // build tiles
    int* _airDamagesBuf;            // build tiles
    bool* _lowResWalkability;       // low res => building tiles
	std::map<BWTA::BaseLocation*, std::vector<BWAPI::TilePosition> > _pathsFromHomeToSL;

    void buildingsAwarePathFind(std::vector<BWAPI::TilePosition>& btpath, 
        const BWAPI::TilePosition& start, const BWAPI::TilePosition& end);
    void damagesAwarePathFindAir(std::vector<BWAPI::TilePosition>& btpath, 
        const BWAPI::TilePosition& start, const BWAPI::TilePosition& end, int damages);
    void damagesAwarePathFindGround(std::vector<BWAPI::TilePosition>& btpath, 
        const BWAPI::TilePosition& start, const BWAPI::TilePosition& end, int damages);
    //void quickPathFind(std::vector<BWAPI::TilePosition>& btpath, // works only with correct Regions
    //    const BWAPI::TilePosition& start,                               // often not the case on custom maps
    //    const BWAPI::TilePosition& end);
    // void pathFind(std::vector<WalkTilePosition>& path, 
    //    const BWAPI::Position& p_start, const BWAPI::Position& p_end);
    void straightLine(std::vector<BWAPI::TilePosition>& btpath, 
        const BWAPI::TilePosition& start, const BWAPI::TilePosition& end);

	std::map<BWTA::Region*, int> regionToHash;
	std::map<BWTA::BaseLocation*, int> baseLocationToHash;
	PathAwareMaps _pfMaps;

public:
    bool* walkability;          // walk tiles
    bool* buildings_wt;         // walk tiles
#ifdef __BUILDINGS_WT_STRICT__
    bool* buildings_wt_strict;  // walk tiles
#endif
    bool* buildings;            // low res => building tiless
    int* groundDamages;         // build tiles
    int* airDamages;            // build tiles
    Vec* groundDamagesGrad;     // build tiles
    Vec* airDamagesGrad;        // build tiles
    std::map<Position, int> stormPos;
    void onUnitCreate(BWAPI::Unit* u);
    void onUnitDestroy(BWAPI::Unit* u);
    void onUnitShow(BWAPI::Unit* u);
    void onUnitHide(BWAPI::Unit* u);
    void justStormed(BWAPI::Position p);
	void pathfind(BayesianUnit* ptr, BWAPI::Unit* u, BWAPI::TilePosition start, BWAPI::TilePosition end);
	void threatAwarePathfind(BayesianUnit* ptr, BWAPI::Unit* u, BWAPI::TilePosition start, BWAPI::TilePosition end, int damages);
	void cancelPathfind(BayesianUnit* ptr);
	const std::vector<BWAPI::TilePosition>& getPathFromHomeToSL(BWTA::BaseLocation* b);
    void update();
    const std::map<BWAPI::Unit*, BWAPI::Position> & getOurUnits();
    const std::map<BWAPI::Unit*, BWAPI::Position> & getTrackedUnits();
    const std::map<BWAPI::Bullet*, BWAPI::Position> & getTrackedStorms();
    BWAPI::Position closestWalkableSameRegionOrConnected(BWAPI::Position p);
	BWAPI::TilePosition closestWalkable(BWAPI::TilePosition tp, BWTA::Region* r);
    BWAPI::TilePosition closestWalkableSameRegionOrConnected(BWAPI::TilePosition tp);
	bool isBTWalkable(int x, int y);
	bool isBTWalkable(const BWAPI::TilePosition& tp);

	BWAPI::Position regionsPFCenters(BWTA::Region* r);
	double distRegions(BWTA::Region* r1, BWTA::Region* r2);
	double distBaseToBase(BWTA::BaseLocation* b1, BWTA::BaseLocation* b2);
	
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