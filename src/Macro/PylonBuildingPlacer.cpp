#include <PrecompiledHeader.h>
#include "Macro/PylonBuildingPlacer.h"
#include "Macro/ReservedMap.h"
#include "Macro/Heap.h"
using namespace std;
using namespace BWAPI;
PylonBuildingPlacer* instance = NULL;
PylonBuildingPlacer* PylonBuildingPlacer::getInstance()
{
  if (instance==NULL)
    instance = new PylonBuildingPlacer();
  return instance;
}
PylonBuildingPlacer::PylonBuildingPlacer()
{
  pylonDistance = 3;
}
void PylonBuildingPlacer::attached(TaskStream* ts)
{
  if (ts->getTask(0).getTilePosition().isValid()==false)
    ts->getTask(0).setTilePosition(Broodwar->self()->getStartLocation());
  taskStreams[ts].isRelocatable   = true;
  taskStreams[ts].buildDistance   = 1;
  taskStreams[ts].reservePosition = ts->getTask(0).getTilePosition();
  taskStreams[ts].reserveWidth    = 0;
  taskStreams[ts].reserveHeight   = 0;
}
void PylonBuildingPlacer::detached(TaskStream* ts)
{
  taskStreams.erase(ts);
}
void PylonBuildingPlacer::newStatus(TaskStream* ts)
{
}
void PylonBuildingPlacer::completedTask(TaskStream* ts, const Task &t)
{
  TheReservedMap->freeTiles(taskStreams[ts].reservePosition,taskStreams[ts].reserveWidth,taskStreams[ts].reserveHeight);
  taskStreams[ts].reserveWidth  = 0;
  taskStreams[ts].reserveHeight = 0;
}
void PylonBuildingPlacer::update(TaskStream* ts)
{
  if (ts->getTask(0).getType()!=TaskTypes::Unit) return;

  int width = ts->getTask(0).getUnit().tileWidth();
  UnitType type = ts->getTask(0).getUnit();
  if (type.isAddon()) type=type.whatBuilds().first;
  //don't look for a build location if this building requires power and we have no pylons
  if (type.requiresPsi() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Pylon)==0) return;
  if (Broodwar->getFrameCount()%10!=0) return;

  if (ts->getStatus()==TaskStream::Error_Location_Blocked || ts->getStatus()==TaskStream::Error_Location_Not_Specified)
  {
    if (ts->getTask(0).getTilePosition().isValid()==false)
      ts->getTask(0).setTilePosition(Broodwar->self()->getStartLocation());
    if (taskStreams[ts].isRelocatable)
    {
      TilePosition tp(ts->getTask(0).getTilePosition());
      
      TilePosition newtp = TilePositions::None;
      int bd = taskStreams[ts].buildDistance;
      while ( newtp == TilePositions::None)
      {
        newtp = getBuildLocationNear(ts->getWorker(),tp,type,bd);
        bd--;
      }
      ts->getTask(0).setTilePosition(newtp);
    }
  }
  if (taskStreams[ts].reserveWidth    != width ||
      taskStreams[ts].reserveHeight   != ts->getTask(0).getUnit().tileHeight() ||
      taskStreams[ts].reservePosition != ts->getTask(0).getTilePosition())
  {
    TheReservedMap->freeTiles(taskStreams[ts].reservePosition,taskStreams[ts].reserveWidth,taskStreams[ts].reserveHeight);
    taskStreams[ts].reserveWidth    = width;
    taskStreams[ts].reserveHeight   = ts->getTask(0).getUnit().tileHeight();
    taskStreams[ts].reservePosition = ts->getTask(0).getTilePosition();
    TheReservedMap->reserveTiles(taskStreams[ts].reservePosition,type,taskStreams[ts].reserveWidth,taskStreams[ts].reserveHeight);
  }
}
void PylonBuildingPlacer::setTilePosition(TaskStream* ts, BWAPI::TilePosition p)
{
  ts->getTask(0).setTilePosition(p);
}
void PylonBuildingPlacer::setRelocatable(TaskStream* ts, bool isRelocatable)
{
  taskStreams[ts].isRelocatable = isRelocatable;
}
void PylonBuildingPlacer::setBuildDistance(TaskStream* ts, int distance)
{
  taskStreams[ts].buildDistance = distance;
}
void PylonBuildingPlacer::setPylonDistance(int pylonDistance)
{
  this->pylonDistance = pylonDistance;
}

BWAPI::TilePosition PylonBuildingPlacer::getBuildLocationNear(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const
{
  //returns a valid build location near the specified tile position.
  //searches outward in a spiral.
  if (type.isAddon()) type = type.whatBuilds().first;
  Heap<TilePosition, int> searchHeap(true);
  searchHeap.push(std::make_pair(position,0));
  std::set< TilePosition > closed;
  while (searchHeap.empty()==false)
  {
    TilePosition t = searchHeap.top().first;
    int s          = searchHeap.top().second;
    searchHeap.pop();
    if (this->canBuildHereWithSpace(builder, t, type, buildDist))
      return t;
    closed.insert(t);
    int tx=t.x();
    int ty=t.y();
    int minx = tx-1; if (minx<0) minx=0;
    int maxx = tx+1; if (maxx>=BWAPI::Broodwar->mapWidth()) maxx=BWAPI::Broodwar->mapWidth()-1;
    int miny = ty-1; if (miny<0) miny=0;
    int maxy = ty+1; if (maxy>=BWAPI::Broodwar->mapHeight()) maxy=BWAPI::Broodwar->mapHeight()-1;
    for(int x=minx;x<=maxx;x++)
    {
      for(int y=miny;y<=maxy;y++)
      {
        if (!Broodwar->isWalkable(x*4,y*4)) continue;
        if (!Broodwar->isWalkable(x*4+3,y*4+3)) continue;
        if (!Broodwar->isWalkable(x*4+3,y*4)) continue;
        if (!Broodwar->isWalkable(x*4,y*4+3)) continue;
        TilePosition t2(x,y);
        if (closed.find(t2)==closed.end())
        {
          int ds=10;
          if (x!=tx && y!=ty) ds=14;
          searchHeap.push(std::make_pair(t2,s+ds));
        }
      }
    }
  }
  if (buildDist>0)
    return getBuildLocationNear(builder, position, type, buildDist-1);
  return BWAPI::TilePositions::None;
}

bool PylonBuildingPlacer::canBuildHere(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const
{
  if (type.isAddon()) type=type.whatBuilds().first;
  //returns true if we can build this type of unit here. Takes into account reserved tiles.
  if (!BWAPI::Broodwar->canBuildHere(builder, position, type))
    return false;
  
  int minx = position.x() - pylonDistance; if (minx<0) minx=0;
  int maxx = position.x() + 2 + pylonDistance; if (maxx>=BWAPI::Broodwar->mapWidth()) maxx=BWAPI::Broodwar->mapWidth()-1;
  int miny = position.y() - pylonDistance; if (miny<0) miny=0;
  int maxy = position.y() + 2 + pylonDistance; if (maxy>=BWAPI::Broodwar->mapHeight()) maxy=BWAPI::Broodwar->mapHeight()-1;
  for(int x=minx;x<=maxx;x++)
    for(int y=miny;y<=maxy;y++)
    {
      if (TheReservedMap->getReservedType(x,y)==UnitTypes::Protoss_Pylon)
        return false;
      for each(Unit* u in Broodwar->getUnitsOnTile(x,y))
        if (u->getPlayer()==Broodwar->self() && u->getType()==UnitTypes::Protoss_Pylon)
          return false;
    }
  return true;
}

bool PylonBuildingPlacer::canBuildHereWithSpace(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const
{
  if (type.isAddon()) type=type.whatBuilds().first;
  //returns true if we can build this type of unit here with the specified amount of space.
  //space value is stored in this->buildDistance.

  //if we can't build here, we of course can't build here with space
  if (!this->canBuildHere(builder,position, type))
    return false;
  if (type.isRefinery())
    return true;

  int width=type.tileWidth();
  int height=type.tileHeight();
  int startx = position.x() - buildDist;
  if (startx<0) return false;
  int starty = position.y() - buildDist;
  if (starty<0) return false;
  int endx = position.x() + width + buildDist;
  if (endx>BWAPI::Broodwar->mapWidth()) return false;
  int endy = position.y() + height + buildDist;
  if (endy>BWAPI::Broodwar->mapHeight()) return false;

  for(int x = startx; x < endx; x++)
    for(int y = starty; y < endy; y++)
      if (!buildable(builder, x, y) || TheReservedMap->isReserved(x,y))
        return false;
  return true;
}

bool PylonBuildingPlacer::buildable(BWAPI::Unit* builder, int x, int y) const
{
  //returns true if this tile is currently buildable, takes into account units on tile
  if (!BWAPI::Broodwar->isBuildable(x,y)) return false;
  std::set<BWAPI::Unit*> units = BWAPI::Broodwar->getUnitsOnTile(x, y);
  for(std::set<BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
    if ((*i)->getType().isBuilding() && !(*i)->isLifted() && !(*i)->getType().isFlyer() && *i != builder)
      return false;
  return true;
}
