#include <PrecompiledHeader.h>
#include "Macro/ReservedMap.h"
using namespace BWAPI;
ReservedMap* TheReservedMap = NULL;
ReservedMap* ReservedMap::create()
{
  if (TheReservedMap) return TheReservedMap;
  return new ReservedMap();
}
void ReservedMap::destroy()
{
  if (TheReservedMap)
    delete TheReservedMap;
}
ReservedMap::ReservedMap()
{
  TheReservedMap = this;
  reserveMap.resize(Broodwar->mapWidth(),Broodwar->mapHeight());
  reserveMap.setTo(UnitTypes::None);
  reserveTiles(Broodwar->self()->getStartLocation(), UnitTypes::Protoss_Nexus, UnitTypes::Protoss_Nexus.tileWidth(), UnitTypes::Protoss_Nexus.tileHeight());
}
ReservedMap::~ReservedMap()
{
  TheReservedMap = NULL;
}
void ReservedMap::reserveTiles(BWAPI::TilePosition position, BWAPI::UnitType type, int width, int height)
{
  for(int x = position.x(); x < position.x() + width && x < (int)reserveMap.getWidth(); x++)
    for(int y = position.y(); y < position.y() + height && y < (int)reserveMap.getHeight(); y++)
      reserveMap[x][y] = type;
}
void ReservedMap::freeTiles(BWAPI::TilePosition position, int width, int height)
{
  for(int x = position.x(); x < position.x() + width && x < (int)reserveMap.getWidth(); x++)
    for(int y = position.y(); y < position.y() + height && y < (int)reserveMap.getHeight(); y++)
      reserveMap[x][y] = UnitTypes::None;
}
bool ReservedMap::isReserved(int x, int y)
{
  return reserveMap[x][y] != UnitTypes::None;
}
bool ReservedMap::isReserved(TilePosition p)
{
  return reserveMap[p.x()][p.y()] != UnitTypes::None;
}
BWAPI::UnitType ReservedMap::getReservedType(int x, int y)
{
  return reserveMap[x][y];
}
BWAPI::UnitType ReservedMap::getReservedType(BWAPI::TilePosition p)
{
  return reserveMap[p.x()][p.y()];
}
