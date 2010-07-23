#pragma once
#include "MapManager.h"
#include "Vec.h"

#include <stdio.h>
#include <string.h>

using namespace BWAPI;

MapManager::MapManager()
{
    _width = 4*Broodwar->mapWidth();
    _height = 4*Broodwar->mapHeight();
    walkability = new bool[_width * _height];             // Walk Tiles resolution
    buildings_wt = new bool[_width * _height];
    buildings_wt_strict = new bool[_width * _height];
    lowResWalkability = new bool[_width * _height / 16];  // Build Tiles resolution
    buildings = new bool[_width * _height / 16];          
    for (unsigned int i = 0; i < 3; ++i)
    {
        vLowResWalkability.push_back(new bool[_width * _height / 16]);
        vBuildings.push_back(new bool[_width * _height / 16]);
    }

    bool* tmp_walkability_med = new bool[_width*_height];
    bool* tmp_walkability_big = new bool[_width*_height];

    // initialization
    for (int x = 0; x < _width; ++x) 
        for (int y = 0; y < _height; ++y) 
        {
            walkability[x + y*_width] = Broodwar->isWalkable(x, y);
            tmp_walkability_med[x + y*_width] = walkability[x + y*_width];
            tmp_walkability_big[x + y*_width] = walkability[x + y*_width];
            buildings_wt[x + y*_width] = false;
        }
    for (int x = 0; x < _width/4; ++x) 
    {
        for (int y = 0; y < _height/4; ++y) 
        {
            int xx = 4*x;
            int yy = 4*y;
            bool walkable = true;
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j) 
                {
                    if (!walkability[xx+i + (yy+j)*_width])
                        walkable = false;
                    for (int ix = xx+i-1; ix < xx+i+1; ++ix)
                    {
                        if (ix < 0 || ix >= _width) continue;
                        for (int iy = yy+j-1; iy < yy+j+1; ++iy)
                        {
                            if (iy < 0 || iy >= _height) continue;
                            if (!walkability[ix + iy*_width]) 
                            {
                                tmp_walkability_med[xx+i + (yy+j)*_width] = false;
                                break;
                            }
                        }
                    }
                }
            }
            lowResWalkability[x + y*_width/4] = walkable;
            vLowResWalkability[0][x + y*_width/4] = walkable;
            buildings[x + y*_width/4] = false; // initialized with manual call to onUnitCreate() in onStart()
            for (std::vector<bool*>::iterator it = vBuildings.begin();
                it != vBuildings.end(); ++it)
                (*it)[x + y*_width/4] = false; // initialized with manual call to onUnitCreate() in onStart()
        }
    }
    for (int x = 0; x < _width/4; ++x)
        for (int y = 0; y < _height/4; ++y)
        {
            int xx = 4*x;
            int yy = 4*y;
            bool walkable_med = true;
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j) 
                {
                    if (!tmp_walkability_med[xx+i + (yy+j)*_width])
                        walkable_med = false;
                    for (int ix = xx+i-1; ix < xx+i+1; ++ix)
                    {
                        if (ix < 0 || ix > _width) continue;
                        for (int iy = yy+j-1; iy < yy+j+1; ++iy)
                        {
                            if (iy < 0 || iy > _height) continue;
                            if (!tmp_walkability_med[ix + iy*_width]) 
                            {
                                tmp_walkability_big[xx+i + (yy+j)*_width] = false;
                                break;
                            }
                        }
                    }
                }
            }
            vLowResWalkability[1][x + y*_width/4] = walkable_med;
        }
    for (int x = 0; x < _width/4; ++x)
        for (int y = 0; y < _height/4; ++y)
        {
            int xx = 4*x;
            int yy = 4*y;
            bool walkable_big = true;
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j) 
                {
                    if (!tmp_walkability_big[xx+i + (yy+j)*_width])
                    {
                        walkable_big = false;
                        break;
                    }
                }
            }
            vLowResWalkability[2][x + y*_width/4] = walkable_big;
        }


    _eUnitsFilter = & EUnitsFilter::Instance();
    delete [] tmp_walkability_med;
    delete [] tmp_walkability_big;
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
        }
    for (int x = tpBd.x()*4 - 1; x < tpBd.x()*4 + u->getType().tileWidth()*4 + 1; ++x)
        for (int y = tpBd.y()*4 - 1; y < tpBd.y()*4 + u->getType().tileHeight()*4 + 1; ++y)
            if (x >= 0 && x < _width && y >= 0 && y < _height)
                buildings_wt[x + y*_width] = b;
    for (int x = (u->getPosition().x() - u->getType().dimensionLeft() - 5) / 8; 
        x <= (u->getPosition().x() + u->getType().dimensionRight() + 5) / 8; ++x)
    {
        for (int y = (u->getPosition().y() - u->getType().dimensionUp() - 5) / 8;
            y <= (u->getPosition().y() + u->getType().dimensionDown() + 5) / 8; ++y)
        {
            buildings_wt_strict[x + y*_width] = b;
            //buildings_wt[x + y*_width] = b;
            //if (y > 0) buildings_wt[x + (y-1)*_width] = b;
        }
    }
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
    for (int x = 0; x < _width; ++x)
        for (int y = 0; y < _height; ++y)
        {
            if (buildings_wt[x + y*_width])
                Broodwar->drawBoxMap(8*x+1, 8*y+1, 8*x+7, 8*y+7, Colors::Orange);
        }
}

void MapManager::drawBuildingsStrict()
{
    for (int x = 0; x < _width; ++x)
        for (int y = 0; y < _height; ++y)
        {
            if (buildings_wt_strict[x + y*_width])
                Broodwar->drawBoxMap(8*x+1, 8*y+1, 8*x+7, 8*y+7, Colors::Orange);
        }
}

void MapManager::drawWalkability()
{
    for (int x = 0; x < _width; ++x)
        for (int y = 0; y < _height; ++y)
        {
            if (!walkability[x + y*_width])
                Broodwar->drawBox(CoordinateType::Map, 8*x + 1, 8*y + 1, 8*x + 7, 8*y + 7, Colors::Red);
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