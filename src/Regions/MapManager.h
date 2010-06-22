#pragma once
#include <BWAPI.h>
#include "CSingleton.h"

/** From BWAPI's doc:
* Positions are measured in pixels and are the highest resolution
* Walk Tiles - each walk tile is an 8x8 square of pixels. These are called walk tiles because walkability data is available at this resolution.
* Build Tiles - each build tile is a 4x4 square of walk tiles, or a 32x32 square of pixels. These are called build tiles because buildability data is available at this resolution. 
*/

// TODO problem when we lift a terran building (we should call onUnitDestroy)
class MapManager: public CSingleton<MapManager>
{
    friend class CSingleton<MapManager>;

private:
    MapManager();
    ~MapManager();
    inline void modifyBuildings(BWAPI::Unit* u, bool b);
    inline void addBuilding(BWAPI::Unit* u);
    inline void removeBuilding(BWAPI::Unit* u);
public:
    bool* walkability;          // walk tiles
    bool* lowResWalkability;    // low res => building tiles
    bool* buildings;            // low res => building tiles
    virtual void onUnitCreate(BWAPI::Unit* u);
    virtual void onUnitDestroy(BWAPI::Unit* u);
    virtual void onUnitShow(BWAPI::Unit* u);
    virtual void onUnitHide(BWAPI::Unit* u);
    void drawBuildings();   // debug
    void drawWalkability(); // debug
};
