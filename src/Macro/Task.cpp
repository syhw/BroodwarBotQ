#include <PrecompiledHeader.h>
#include "Macro/Task.h"
using namespace BWAPI;

Task::Task()
{
  type                        = TaskTypes::None;
  id                          = -1;
  position                    = TilePositions::None;
  level                       = -1;
  startTime                   = -1;
  earliestStartTime           = 0;
  spentResources              = false;
  reservedResourcesThisFrame  = false;
  reservedFinishDataThisFrame = false;
  completed                   = false;
  executing                   = false;
}
Task::Task(const BWAPI::UnitType t, const BWAPI::TilePosition p)
{
  if (t==UnitTypes::None || t==UnitTypes::Unknown)
  {
    type     = TaskTypes::None;
    id       = -1;
  }
  else
  {
    type     = TaskTypes::Unit;
    id       = t.getID();
  }
  position                    = p;
  level                       = -1;
  startTime                   = -1;
  earliestStartTime           = 0;
  spentResources              = false;
  reservedResourcesThisFrame  = false;
  reservedFinishDataThisFrame = false;
  completed                   = false;
  executing                   = false;
}
Task::Task(const BWAPI::TechType t, const BWAPI::TilePosition p)
{
  if (t==TechTypes::None || t==TechTypes::Unknown)
  {
    type     = TaskTypes::None;
    id       = -1;
  }
  else
  {
    type     = TaskTypes::Tech;
    id       = t.getID();
  }
  position                    = p;
  level                       = -1;
  startTime                   = -1;
  earliestStartTime           = 0;
  spentResources              = false;
  reservedResourcesThisFrame  = false;
  reservedFinishDataThisFrame = false;
  completed                   = false;
  executing                   = false;
}
Task::Task(const BWAPI::UpgradeType t, int l, const BWAPI::TilePosition p)
{
  if (t==UpgradeTypes::None || t==UpgradeTypes::Unknown)
  {
    type     = TaskTypes::None;
    id       = -1;
  }
  else
  {
    type     = TaskTypes::Upgrade;
    id       = t.getID();
  }
  position                    = p;
  if (l==-1)
    l=Broodwar->self()->getUpgradeLevel(t)+1;
  level                       = l;
  startTime                   = -1;
  earliestStartTime           = 0;
  spentResources              = false;
  reservedResourcesThisFrame  = false;
  reservedFinishDataThisFrame = false;
  completed                   = false;
  executing                   = false;
}
Task& Task::operator=(const Task t)
{
  type                        = t.type;
  id                          = t.id;
  position                    = t.position;
  level                       = t.level;
  startTime                   = t.startTime;
  earliestStartTime           = t.earliestStartTime;
  spentResources              = t.spentResources;
  reservedResourcesThisFrame  = t.reservedResourcesThisFrame;
  reservedFinishDataThisFrame = t.reservedFinishDataThisFrame;
  completed                   = t.completed;
  executing                   = t.executing;
  return *this;
}
Task& Task::setType(const BWAPI::UnitType t,    const BWAPI::TilePosition p)
{
  if (t==UnitTypes::None || t==UnitTypes::Unknown)
  {
    type     = TaskTypes::None;
    id       = -1;
  }
  else
  {
    type     = TaskTypes::Unit;
    id       = t.getID();
  }
  if (p!= BWAPI::TilePositions::None)
    position = p;
  return *this;
}
Task& Task::setType(const BWAPI::TechType t,    const BWAPI::TilePosition p)
{
  if (t==TechTypes::None || t==TechTypes::Unknown)
  {
    type     = TaskTypes::None;
    id       = -1;
  }
  else
  {
    type     = TaskTypes::Tech;
    id       = t.getID();
  }
  if (p!= BWAPI::TilePositions::None)
    position = p;
  return *this;
}
Task& Task::setType(const BWAPI::UpgradeType t, int l,    const BWAPI::TilePosition p)
{
  if (t==UpgradeTypes::None || t==UpgradeTypes::Unknown)
  {
    type     = TaskTypes::None;
    id       = -1;
  }
  else
  {
    type     = TaskTypes::Upgrade;
    id       = t.getID();
  }
  if (l!=-1)
    level  = l;
  if (p!= BWAPI::TilePositions::None)
    position = p;
  return *this;
}
Task& Task::setLevel(int l)
{
  level = l;
  return *this;
}

Task& Task::setTilePosition(const BWAPI::TilePosition p)
{
  position = p;
  return *this;
}
bool Task::operator==(void* ptr) const
{
  if (ptr) return false;
  if (*this == Task())
    return true;
  return false;
}
bool Task::operator!=(void* ptr) const
{
  return !(this==ptr);
}
bool Task::operator==(const Task &t) const
{
  if (type                        != t.type) return false;
  if (id                          != t.id) return false;
  if (position                    != t.position) return false;
  if (level                       != t.level) return false;
  if (startTime                   != t.startTime) return false;
  if (earliestStartTime           != t.earliestStartTime) return false;
  if (spentResources              != t.spentResources) return false;
  if (reservedResourcesThisFrame  != t.reservedResourcesThisFrame) return false;
  if (reservedFinishDataThisFrame != t.reservedFinishDataThisFrame) return false;
  if (completed                   != t.completed) return false;
  if (executing                   != t.executing) return false;
  return true;
}
bool Task::operator<(const Task &t) const
{
  if (type<t.type) return true;
  if (type>t.type) return false;
  if (id<t.id) return true;
  if (id>t.id) return false;
  if (position<t.position) return true;
  if (t.position<position) return false;
  if (level<t.level) return true;
  if (level>t.level) return false;
  if (startTime<t.startTime) return true;
  if (startTime>t.startTime) return false;
  if (earliestStartTime<t.earliestStartTime) return true;
  if (earliestStartTime>t.earliestStartTime) return false;
  if (spentResources<t.spentResources) return true;
  if (spentResources>t.spentResources) return false;
  if (reservedResourcesThisFrame<t.reservedResourcesThisFrame) return true;
  if (reservedResourcesThisFrame>t.reservedResourcesThisFrame) return false;
  if (reservedFinishDataThisFrame<t.reservedFinishDataThisFrame) return true;
  if (reservedFinishDataThisFrame>t.reservedFinishDataThisFrame) return false;
  if (completed<t.completed) return true;
  if (completed>t.completed) return false;
  if (executing<t.executing) return true;
  if (executing>t.executing) return false;
  return false;
}
bool Task::operator==(const BWAPI::UnitType &t) const
{
  return (type == TaskTypes::Unit && id == t.getID());
}
bool Task::operator==(const BWAPI::TechType &t) const
{
  return (type == TaskTypes::Tech && id == t.getID());
}
bool Task::operator==(const BWAPI::UpgradeType &t) const
{
  return (type == TaskTypes::Upgrade && id == t.getID());
}
bool Task::operator==(const BWAPI::TilePosition &p) const
{
  return position==p;
}

TaskTypes::Enum Task::getType() const
{
  return type;
}
BWAPI::UnitType Task::getUnit() const
{
  if (type==TaskTypes::Unit)
    return UnitType(id);
  return UnitTypes::None;
}
BWAPI::TechType Task::getTech() const
{
  if (type==TaskTypes::Tech)
    return TechType(id);
  return TechTypes::None;
}
BWAPI::UpgradeType Task::getUpgrade() const
{
  if (type==TaskTypes::Upgrade)
    return UpgradeType(id);
  return UpgradeTypes::None;
}
int Task::getLevel() const
{
  return level;
}
BWAPI::TilePosition Task::getTilePosition() const
{
  return position;
}
BWAPI::UnitType Task::getWorkerType() const
{
  if (type == TaskTypes::Unit)
    return UnitType(id).whatBuilds().first;
  if (type == TaskTypes::Tech)
    return TechType(id).whatResearches();
  if (type == TaskTypes::Upgrade)
    return UpgradeType(id).whatUpgrades();
  return UnitTypes::None;
}
BWAPI::Race Task::getRace() const
{
  return getWorkerType().getRace();
}

std::map<BWAPI::UnitType, int> Task::getRequiredUnits() const
{
  std::map<BWAPI::UnitType, int> r;
  if (type == TaskTypes::Unit)
    r=UnitType(id).requiredUnits();
  else if (type == TaskTypes::Tech)
    r.insert(std::make_pair(TechType(id).whatResearches(),1));
  else if (type == TaskTypes::Upgrade)
    r.insert(std::make_pair(UpgradeType(id).whatUpgrades(),1));
  return r;
}
Resources Task::getResources() const
{
  if (type == TaskTypes::Unit)
    return Resources(UnitType(id));
  if (type == TaskTypes::Tech)
    return Resources(TechType(id));
  if (type == TaskTypes::Upgrade)
    return Resources(UpgradeType(id),level);
  return Resources();
}
int Task::getTime() const
{
  if (type == TaskTypes::Unit)
    return UnitType(id).buildTime();
  if (type == TaskTypes::Tech)
    return TechType(id).researchTime();
  if (type == TaskTypes::Upgrade)
    return UpgradeType(id).upgradeTime()+UpgradeType(id).upgradeTimeFactor()*(level-1);
  return 0;
}
std::string Task::getName() const
{
  if (type == TaskTypes::Unit)
    return UnitType(id).getName();
  if (type == TaskTypes::Tech)
    return TechType(id).getName();
  if (type == TaskTypes::Upgrade)
    return UpgradeType(id).getName();
  return "None";
}
std::string Task::getVerb() const
{
  if (type == TaskTypes::Unit)
    return "Build";
  if (type == TaskTypes::Tech)
    return "Research";
  if (type == TaskTypes::Upgrade)
    return "Upgrade";
  return "";
}
bool Task::isBeingExecutedBy(const BWAPI::Unit* unit) const
{
  if (type == TaskTypes::Unit)
  {
    UnitType ut=getUnit();
    if (ut.getRace()==Races::Zerg && ut.isBuilding()==ut.whatBuilds().first.isBuilding())
      return (unit->isMorphing() && unit->getBuildType() == ut);
    UnitType buildType = unit->getBuildType();
    if (buildType == UnitTypes::None && unit->getBuildUnit()!=NULL)
      buildType = unit->getBuildUnit()->getType();

    if (ut.isBuilding())
      return buildType == ut;
    else
      return (unit->isTraining() && unit->getTrainingQueue().front() == ut);
  }
  else if (type == TaskTypes::Tech)
    return (unit->isResearching() && unit->getTech() == getTech());
  else if (type == TaskTypes::Upgrade)
    return (unit->isUpgrading() && unit->getUpgrade() == getUpgrade());
  return false;
}
void Task::setEarliestStartTime(int time)
{
  earliestStartTime = time;
}
int Task::getEarliestStartTime() const
{
  return earliestStartTime;
}
void Task::setStartTime(int time)
{
  startTime = time;
}
int Task::getStartTime() const
{
  return startTime;
}
int Task::getFinishTime() const
{
  if (startTime<0) return -1;
  return startTime+getTime();
}

int Task::getRemainingTime() const
{
  if (startTime<0) return -1;
  int t=0;
  int timePassed=Broodwar->getFrameCount()-startTime;
  if (timePassed<0) timePassed=0;
  t=getTime()-timePassed;
  if (t<0) return 0;
  return t;
}

void Task::setSpentResources(bool spent)
{
  spentResources = spent;
}
bool Task::hasSpentResources() const
{
  return spentResources;
}
void Task::setExecuting(bool exec)
{
  executing = exec;
}
bool Task::isExecuting() const
{
  return executing;
}
void Task::setReservedResourcesThisFrame(bool reserved)
{
  reservedResourcesThisFrame = reserved;
}
bool Task::hasReservedResourcesThisFrame() const
{
  return reservedResourcesThisFrame;
}
void Task::setReservedFinishDataThisFrame(bool reserved)
{
  reservedFinishDataThisFrame = reserved;
}
bool Task::hasReservedFinishDataThisFrame() const
{
  return reservedFinishDataThisFrame;
}
void Task::setCompleted(bool c)
{
  completed = c;

}
bool Task::isCompleted() const
{
  return completed;
}