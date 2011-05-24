#pragma once
#include <MacroManager/TaskStreamObserver.h>
#include <MacroManager/TaskStream.h>
#include <RectangleArray.h>
class PylonBuildingPlacer : public TaskStreamObserver
{
  public:
    static PylonBuildingPlacer* getInstance();
    PylonBuildingPlacer();
    virtual void attached(TaskStream* ts);
    virtual void detached(TaskStream* ts);
    virtual void newStatus(TaskStream* ts);
    virtual void completedTask(TaskStream* ts, const Task &t);
    virtual void update(TaskStream* ts);
    void setTilePosition(TaskStream* ts, BWAPI::TilePosition p);
    void setRelocatable(TaskStream* ts, bool isRelocatable);
    void setBuildDistance(TaskStream* ts, int distance);
    void setPylonDistance(int pylonDistance);
  private:
    BWAPI::TilePosition getBuildLocationNear(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const;
    bool canBuildHere(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const;
    bool canBuildHereWithSpace(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const;
    bool buildable(BWAPI::Unit* builder, int x, int y) const;
    struct data
    {
      bool isRelocatable;
      int buildDistance;
      BWAPI::TilePosition reservePosition;
      int reserveWidth;
      int reserveHeight;
    };
    int pylonDistance;
    std::map< TaskStream*, data > taskStreams;

};
