#pragma once
#include <CSingleton.h>
#include <BWAPI.h>
#include "RectangleArray.h"

class BuildingPlacer: public CSingleton<BuildingPlacer>
{
	friend class CSingleton<BuildingPlacer>;

	private:
		BuildingPlacer();
		~BuildingPlacer();

  public:
    bool canBuildHere(BWAPI::TilePosition position, BWAPI::UnitType type) const;
    bool canBuildHereWithSpace(BWAPI::TilePosition position, BWAPI::UnitType type) const;
    BWAPI::TilePosition getBuildLocation(BWAPI::UnitType type) const;
    BWAPI::TilePosition getBuildLocationNear(BWAPI::TilePosition position,BWAPI::UnitType type) const;
    bool buildable(int x, int y) const;
    void reserveTiles(BWAPI::TilePosition position, int width, int height);
    void freeTiles(BWAPI::TilePosition position, int width, int height);
    void setBuildDistance(int distance);
    int getBuildDistance();
  private:
    Util::RectangleArray<bool> reserveMap;
    int buildDistance;
};