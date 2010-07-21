#pragma once
#include "MapManager.h"
#include "Vec.h"
using namespace BWAPI;

MapManager::MapManager()
{
    const int width = 4*Broodwar->mapWidth();
    const int height = 4*Broodwar->mapHeight();
    walkability = new bool[width * height];             // Walk Tiles resolution
    buildings_wt = new bool[width * height];
    lowResWalkability = new bool[width * height / 16];  // Build Tiles resolution
    buildings = new bool[width * height / 16];          // Build Tiles resolution
    for (unsigned int i = 0; i < 3; ++i)
    {
        vLowResWalkability.push_back(new bool[width * height / 16]);
        vBuildings.push_back(new bool[width * height / 16]);
    }

    // initialization
    for (int x = 0; x < width; ++x) 
        for (int y = 0; y < height; ++y) 
        {
            walkability[x + y*width] = Broodwar->isWalkable(x, y);
            buildings_wt[x + y*width] = false;
        }
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
            vLowResWalkability[0][x + y*width/4] = walkable;
            buildings[x + y*width/4] = false; // initialized with manual call to onUnitCreate() in onStart()
            for (std::vector<bool*>::iterator it = vBuildings.begin();
                it != vBuildings.end(); ++it)
                (*it)[x + y*width/4] = false; // initialized with manual call to onUnitCreate() in onStart()
        }
    }
    for (int x = 0; x < width/4; ++x)
        for (int y = 0; y < height/4; ++y)
        {
            bool walkable_med = true;
            for (Vec v1(4*x-4, 4*y-1), Vec v2(4*x-3, 4*y-2), Vec v3(4*x-2, 4*y-3), Vec v4(4*x-1, 4*y-4), unsigned int i = 0;
                    i < 8; ++v1, ++v2, ++v3, ++v4, ++i)
            {
                if (!walkability[v1.x + v1.y * width] || !walkability[v2.x + v2.y * width] 
                    || !walkability[v3.x + v3.y * width] || !walkability[v4.x + v4.y * width])
                        walkable_med = false;
            }
            for (Vec v1(4*x+3+4, 4*y-1), Vec v2(4*x+3+3, 4*y-2), Vec v3(4*x+3+2, 4*y-3), Vec v4(4*x+3+1, 4*y-4), unsigned int i = 0;
                i < 8; --v1, --v2, --v3, --v4, ++i)
            {
                if (!walkability[v1.x + v1.y * width] || !walkability[v2.x + v2.y * width] 
                || !walkability[v3.x + v3.y * width] || !walkability[v4.x + v4.y * width])
                        walkable_med = false;
            }

            bool walkable_big = true;
            if (x + 1 > width/4 || y + 1 > height/4)
                walkable_big = false;
            else
            {
                for (unsigned int i = 0; i < 2; ++i)
                    for (unsigned int j = 0; j < 2; ++j)
                        if (!lowResWalkability[x+i + (y+j)*width/4])
                            walkable_big = false;
            }
            vLowResWalkability[1][x + y*width/4] = walkable_med;
            vLowResWalkability[2][x + y*width/4] = walkable_big;
        }
    
    /*for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            lowResWalkability[x/4 + y/4 * width/4] &= walkability[x + y * width];
            buildings[x/4 + y/4 * width/4] = false;
        }
    }*/
    _eUnitsFilter = & EUnitsFilter::Instance();
}

MapManager::~MapManager()
{
    delete [] walkability;
    delete [] lowResWalkability;
    delete [] buildings_wt;
    delete [] buildings;
    for (std::vector<bool*>::iterator it = vLowResWalkability.begin();
        it != vLowResWalkability.end(); ++it)
        delete [] *it;
    for (std::vector<bool*>::iterator it = vBuildings.begin();
        it != vBuildings.end(); ++it)
        delete [] *it;
}

void MapManager::modifyBuildings(Unit* u, bool b)
{
    if (!u->getType().isBuilding() || (u->isLifted() && b)) return;
    TilePosition tpBd = u->getTilePosition(); // top left corner of the building
    for (int x = tpBd.x(); x < tpBd.x() + u->getType().tileWidth(); ++x)
        for (int y = tpBd.y(); y < tpBd.y() + u->getType().tileHeight(); ++y)
        {
            buildings[x + y*Broodwar->mapWidth()] = b;
            vBuildings[0][x + y*Broodwar->mapWidth()] = b;
        }
    for (int x = tpBd.x()*4 - 1; x < tpBd.x()*4 + u->getType().tileWidth()*4 + 1; ++x)
        for (int y = tpBd.y()*4 - 1; y < tpBd.y()*4 + u->getType().tileHeight()*4 + 1; ++y)
            buildings_wt[x + y*Broodwar->mapWidth()*4] = b;
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

void MapManager::onFrame()
{

}

void MapManager::drawBuildings()
{
    for (int x = 0; x < Broodwar->mapWidth()*4; ++x)
        for (int y = 0; y < Broodwar->mapHeight()*4; ++y)
        {
            if (buildings_wt[x + y*Broodwar->mapWidth()*4])
                Broodwar->drawBoxMap(8*x+1, 8*y+1, 8*x+7, 8*y+7, Colors::Orange);
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

void MapManager::drawBuildings(unsigned int i)
{
    for (int x = 0; x < Broodwar->mapWidth(); ++x)
        for (int y = 0; y < Broodwar->mapHeight(); ++y)
        {
            if (vBuildings[i][x + y*Broodwar->mapWidth()])
                Broodwar->drawBoxMap(32*x + 2, 32*y + 2, 32*x + 30, 32*y + 30, Colors::Orange);
        }
}

void MapManager::drawWalkability(unsigned int i)
{
    for (int x = 0; x < Broodwar->mapWidth(); ++x)
        for (int y = 0; y < Broodwar->mapHeight(); ++y)
        {
            if (!vLowResWalkability[i][x + y*Broodwar->mapWidth()])
                Broodwar->drawBox(CoordinateType::Map, 32*x + 2, 32*y + 2, 32*x + 30, 32*y + 30, Colors::Red);
        }
}

void MapManager::drawLowResWalkability()
{
    for (int x = 0; x < Broodwar->mapWidth(); ++x)
        for (int y = 0; y < Broodwar->mapHeight(); ++y)
        {
            if (!lowResWalkability[x + y*Broodwar->mapWidth()])
                Broodwar->drawBox(CoordinateType::Map, 32*x + 2, 32*y + 2, 32*x + 30, 32*y + 30, Colors::Red);
        }
}

void MapManager::drawLowResBuildings()
{
    for (int x = 0; x < Broodwar->mapWidth(); ++x)
        for (int y = 0; y < Broodwar->mapHeight(); ++y)
        {
            if (buildings[x + y*Broodwar->mapWidth()])
                Broodwar->drawBox(CoordinateType::Map, 32*x + 2, 32*y + 2, 32*x + 30, 32*y + 30, Colors::Orange);
            //Broodwar->drawBox(CoordinateType::Map, 32*x - 15, 32*y - 15, 32*x + 15, 32*y + 15, Colors::Orange);
        }
}