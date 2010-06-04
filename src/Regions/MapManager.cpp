#pragma once
#include "MapManager.h"
using namespace BWAPI;

MapManager::MapManager()
{
    const int width = 4*Broodwar->mapWidth();
    const int height = 4*Broodwar->mapHeight();
    walkability = new bool[width * height];             // Walk Tiles resolution
    lowResWalkability = new bool[width * height / 16];  // Build Tiles resolution
    buildings = new bool[width * height / 16];          // Build Tiles resolution
    for (int x = 0; x < width; ++x) 
        for (int y = 0; y < height; ++y) 
            walkability[x + y*width] = Broodwar->isWalkable(x, y);
    for (int x = 0; x < width/4; ++x) 
    {
        for (int y = 0; y < height/4; ++y) 
        {
            int xx = 4*x;
            int yy = 4*y;
            bool walkable = true;
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j) 
                {
                    if (!walkability[xx+i + (yy+j)*width])
                        walkable = false;
                }
            }
            lowResWalkability[x + y*width/4] = walkable;
            buildings[x + y*width/4] = false; // initialized with manual call to onUnitCreate() in onStart()
        }
    }
}

MapManager::~MapManager()
{
    delete [] walkability;
}

void MapManager::modifyBuildings(Unit* u, bool b)
{
    if (!u->getType().isBuilding() || (u->isLifted() && b)) return;
    TilePosition tpBd = u->getTilePosition(); // top left corner of the building
    for (int x = tpBd.x(); x < tpBd.x() + u->getType().tileWidth(); ++x)
        for (int y = tpBd.y(); y < tpBd.y() + u->getType().tileHeight(); ++y)
            buildings[x + y*Broodwar->mapWidth()] = b;
}

void MapManager::addBuilding(Unit* u)
{
    // Broodwar->printf("modified buildings\n");
    modifyBuildings(u, true);
}

void MapManager::removeBuilding(Unit* u)
{
    modifyBuildings(u, false);
}

void MapManager::onUnitCreate(Unit* u)
{

    addBuilding(u);
}

void MapManager::onUnitDestroy(Unit* u)
{
    removeBuilding(u);
}

void MapManager::onUnitShow(Unit* u)
{
    addBuilding(u);
}

void MapManager::onUnitHide(Unit* u)
{
    addBuilding(u);
}

void MapManager::drawBuildings()
{
    for (int x = 0; x < Broodwar->mapWidth(); ++x)
        for (int y = 0; y < Broodwar->mapHeight(); ++y)
        {
            if (buildings[x + y*Broodwar->mapWidth()])
                Broodwar->drawBox(CoordinateType::Map, 32*x - 15, 32*y - 15, 32*x + 15, 32*y + 15, Colors::Orange);
        }
}

void MapManager::drawWalkability()
{
    for (int x = 0; x < Broodwar->mapWidth()*4; ++x)
        for (int y = 0; y < Broodwar->mapHeight()*4; ++y)
        {
            if (!walkability[x + y*Broodwar->mapWidth()*4])
                Broodwar->drawBox(CoordinateType::Map, 8*x - 3, 8*y - 3, 8*x + 3, 8*y + 3, Colors::Red);
        }
}