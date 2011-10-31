#pragma once
#include <BWAPI.h>
class BuildManager;
class UnitItem
{
  public:
    UnitItem();
    UnitItem(BWAPI::UnitType type);
    int getRemainingCount(int currentPlannedCount=-1);
    void addAdditional(int count, BWAPI::TilePosition position);
    void setNonAdditional(int count, BWAPI::TilePosition position);
    BWAPI::TilePosition decrementAdditional(BWAPI::TilePosition position=BWAPI::TilePositions::None);
    static BuildManager*& getBuildManager();

  private:
    BWAPI::UnitType type;
    std::map<BWAPI::TilePosition, int> additional;
    int nonadditional;
    BWAPI::TilePosition nonadditionalPosition;
};
