#pragma once
#include <BWAPI.h>
#include <Resources.h>
namespace TaskTypes
{
  enum Enum
  {
    None,
    Unit,
    Tech,
    Upgrade
  };
}
/* A Task object is either a UnitType, TechType, or UpgradeType (+ level) with state information about the current status of the Task
   May want to split into TaskType class (purely unit/upgrade/tech type information) and Task class (holds TaskType + status information) in the future,
   but this works for now.
*/
class Task
{
  public:
    Task();
    Task(const BWAPI::UnitType t,                             const BWAPI::TilePosition p = BWAPI::TilePositions::None);
    Task(const BWAPI::TechType t,                             const BWAPI::TilePosition p = BWAPI::TilePositions::None);
    Task(const BWAPI::UpgradeType t, int l = -1,              const BWAPI::TilePosition p = BWAPI::TilePositions::None);

    Task& operator=(const Task t);
    Task& setType(const BWAPI::UnitType t,    const BWAPI::TilePosition p = BWAPI::TilePositions::None);
    Task& setType(const BWAPI::TechType t,    const BWAPI::TilePosition p = BWAPI::TilePositions::None);
    Task& setType(const BWAPI::UpgradeType t, int l = -1,    const BWAPI::TilePosition p = BWAPI::TilePositions::None);
    Task& setLevel(int l);
    Task& setTilePosition(const BWAPI::TilePosition p);

    bool operator==(void* ptr) const;
    bool operator!=(void* ptr) const;
    bool operator==(const Task &t) const;
    bool operator<(const Task &t) const;
    bool operator==(const BWAPI::UnitType &t) const;
    bool operator==(const BWAPI::TechType &t) const;
    bool operator==(const BWAPI::UpgradeType &t) const;
    bool operator==(const BWAPI::TilePosition &p) const;

    /* Returns true if the given unit is executing this task */
    bool isBeingExecutedBy(const BWAPI::Unit* unit) const;

    /* Returns TaskTypes::Unit, TaskTypes::Tech, or TaskTypes::Upgrade */
    TaskTypes::Enum getType() const;

    /** Returns UnitType if Task is a unit, or UnitTypes::None */
    BWAPI::UnitType getUnit() const;

    /** Returns the TechType if Task is a tech, or TechTypes::None */
    BWAPI::TechType getTech() const;

    /** Returns the UpgradeType if Task is an upgrade, or UpgradeTypes::None */
    BWAPI::UpgradeType getUpgrade() const;

    /** Level (if upgrade) */
    int getLevel() const;

    /** Tile Position (if building/add-on) */
    BWAPI::TilePosition getTilePosition() const;

    /** whatBuilds, whatResearches, or whatUpgrades */
    BWAPI::UnitType getWorkerType() const;

    /** Race of the worker type */
    BWAPI::Race getRace() const;

    /** requiredUnits, whatResearches, or whatUpgrades */
    std::map<BWAPI::UnitType, int> getRequiredUnits() const;

    /* Cost of the UnitType, TechType, or UpgradeType (incl level) */
    Resources getResources() const;

    /* Build Time, Research Time, or Upgrade Time (incl level) */
    int getTime() const;

    /* Name of the UnitType, TechType, or UpgradeType */
    std::string getName() const;

    /** Build, Research, Upgrade */
    std::string getVerb() const;

    /** set/get the earliest start time for this task*/
    void setEarliestStartTime(int time);
    int getEarliestStartTime() const;

    /** set/get the actual start time for this task */
    void setStartTime(int time);
    int getStartTime() const;
    /** finish time = start time + getTime(), note: not accurate for halted constructions */
    int getFinishTime() const;

    /** Remaining time until this task is complete */
    int getRemainingTime() const;

    void setSpentResources(bool spent);
    bool hasSpentResources() const;

    void setExecuting(bool exec);
    bool isExecuting() const;

    void setReservedResourcesThisFrame(bool reserved);
    bool hasReservedResourcesThisFrame() const;

    void setReservedFinishDataThisFrame(bool reserved);
    bool hasReservedFinishDataThisFrame() const;

    void setCompleted(bool c);
    bool isCompleted() const;

  private:
    TaskTypes::Enum type;
    int id;
    int level;
    BWAPI::TilePosition position;
    int earliestStartTime;
    int startTime;
    bool spentResources;
    bool reservedResourcesThisFrame;
    bool reservedFinishDataThisFrame;
    bool completed;
    bool executing;
};

