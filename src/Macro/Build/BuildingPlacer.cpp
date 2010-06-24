#include <BuildingPlacer.h>
BuildingPlacer::BuildingPlacer()
{
  reserveMap.resize(BWAPI::Broodwar->mapWidth(), BWAPI::Broodwar->mapHeight());
  reserveMap.setTo(false);
  this->buildDistance=1;
}

BuildingPlacer::~BuildingPlacer()
{
}

bool BuildingPlacer::canBuildHere(BWAPI::TilePosition position, BWAPI::UnitType type) const
{
  if (!BWAPI::Broodwar->canBuildHere(NULL, position, type))
    return false;
  for(int x = position.x(); x < position.x() + type.tileWidth(); x++)
    for(int y = position.y(); y < position.y() + type.tileHeight(); y++)
      if (reserveMap[x][y])
        return false;
  return true;
}

bool BuildingPlacer::canBuildHereWithSpace(BWAPI::TilePosition position, BWAPI::UnitType type) const
{
  if (!this->canBuildHere(position, type))
    return false;
  int width=type.tileWidth();
  int height=type.tileHeight();
  if (type==BWAPI::UnitTypes::Terran_Command_Center ||
    type==BWAPI::UnitTypes::Terran_Factory || 
    type==BWAPI::UnitTypes::Terran_Starport ||
    type==BWAPI::UnitTypes::Terran_Science_Facility)
  {
    width+=2;
  }
  int startx = position.x() - buildDistance;
  if (startx<0) startx=0;
  int starty = position.y() - buildDistance;
  if (starty<0) starty=0;
  int endx = position.x() + width + buildDistance;
  if (endx>BWAPI::Broodwar->mapWidth()) endx=BWAPI::Broodwar->mapWidth();
  int endy = position.y() + height + buildDistance;
  if (endy>BWAPI::Broodwar->mapHeight()) endy=BWAPI::Broodwar->mapHeight();

  for(int x = startx; x < endx; x++)
    for(int y = starty; y < endy; y++)
      if (!type.isRefinery())
        if (!buildable(x, y))
          return false;

  if (position.x()>3)
  {
    int startx2=startx-2;
    if (startx2<0) startx2=0;
    for(int x = startx2; x < startx; x++)
      for(int y = starty; y < endy; y++)
      {
        std::set<BWAPI::Unit*> units = BWAPI::Broodwar->unitsOnTile(x, y);
        for(std::set<BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
        {
          if (!(*i)->isLifted())
          {
            BWAPI::UnitType type=(*i)->getType();
            if (type==BWAPI::UnitTypes::Terran_Command_Center ||
              type==BWAPI::UnitTypes::Terran_Factory || 
              type==BWAPI::UnitTypes::Terran_Starport ||
              type==BWAPI::UnitTypes::Terran_Science_Facility)
            {
              return false;
            }
          }
        }
      }
  }
  return true;
}
BWAPI::TilePosition BuildingPlacer::getBuildLocation(BWAPI::UnitType type) const
{
  for(int x = 0; x < BWAPI::Broodwar->mapWidth(); x++)
    for(int y = 0; y < BWAPI::Broodwar->mapHeight(); y++)
      if (this->canBuildHere(BWAPI::TilePosition(x, y), type))
        return BWAPI::TilePosition(x, y);
  return BWAPI::TilePositions::None;
}
BWAPI::TilePosition BuildingPlacer::getBuildLocationNear(BWAPI::TilePosition position, BWAPI::UnitType type) const
{
  int x      = position.x();
  int y      = position.y();
  int length = 1;
  int j      = 0;
  bool first = true;
  int dx     = 0;
  int dy     = 1;
  while (length < BWAPI::Broodwar->mapWidth())
  {
    if (x >= 0 && x < BWAPI::Broodwar->mapWidth() && y >= 0 && y < BWAPI::Broodwar->mapHeight())
      if (this->canBuildHereWithSpace(BWAPI::TilePosition(x, y), type))
        return BWAPI::TilePosition(x, y);
    x = x + dx;
    y = y + dy;
    j++;
    if (j == length)
    {
      j = 0;
      if (!first)
        length++;
      first =! first;
      if (dx == 0)
      {
        dx = dy;
        dy = 0;
      }
      else
      {
        dy = -dx;
        dx = 0;
      }
    }
  }
  return BWAPI::TilePositions::None;
}

bool BuildingPlacer::buildable(int x, int y) const
{
	if (!BWAPI::Broodwar->isBuildable(x,y)) return false;
  std::set<BWAPI::Unit*> units = BWAPI::Broodwar->unitsOnTile(x, y);
  for(std::set<BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
    if ((*i)->getType().isBuilding() && !(*i)->isLifted())
      return false;
  return true;
}

void BuildingPlacer::reserveTiles(BWAPI::TilePosition position, int width, int height)
{
  for(int x = position.x(); x < position.x() + width && x < (int)reserveMap.getWidth(); x++)
    for(int y = position.y(); y < position.y() + height && y < (int)reserveMap.getHeight(); y++)
      reserveMap[x][y] = true;
}

void BuildingPlacer::freeTiles(BWAPI::TilePosition position, int width, int height)
{
  for(int x = position.x(); x < position.x() + width && x < (int)reserveMap.getWidth(); x++)
    for(int y = position.y(); y < position.y() + height && y < (int)reserveMap.getHeight(); y++)
      reserveMap[x][y] = false;
}

void BuildingPlacer::setBuildDistance(int distance)
{
  this->buildDistance=distance;
}
int BuildingPlacer::getBuildDistance()
{
  return this->buildDistance;
}
