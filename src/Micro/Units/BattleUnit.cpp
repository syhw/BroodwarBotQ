#include "BattleUnit.h"
#include "Vec.h"
#include "Defines.h"
#include "MapManager.h"
#include <map>
#include <stdio.h>
using namespace BWAPI;

//////////////////////////////////////////////////
////////            BattleUnit             ///////
//////////////////////////////////////////////////

BattleUnit::BattleUnit(BWAPI::Unit* unit)
: unit(unit)
, timeIdle(0)
, _tick(0)
, targetEnemy(NULL)
, targetEnemyInRange(NULL)
, target(unit->getPosition())
, _sheight(unit->getType().dimensionUp() + unit->getType().dimensionDown())
, _slarge(unit->getType().dimensionRight() + unit->getType().dimensionLeft())
#ifdef UNIT_DEBUG
, _unitType(unit->getType().getName())
#endif
{
}

BattleUnit::~BattleUnit()
{
}

void BattleUnit::attackMove(const Position& p)
{
    target = p;
    //pathFind(_path, unit->getPosition(), target);
    /*std::vector<TilePosition> path;
    pathFind(path, unit->getPosition(), target);
    path = std::vector<TilePosition>(getShortestPath(
        unit->getTilePosition(), target));
    if (path.size() == 0) 
        Broodwar->printf("Scandale path.size()==0");*/
    //unit->rightClick(path.pop....
	unit->attackMove(p);
}

bool BattleUnit::operator == (const BattleUnit& bu) const
{
	return (unit == bu.unit) && (target == bu.target);
}

void BattleUnit::drawVelocityArrow()
{
    int xfrom = unit->getPosition().x();
    int yfrom = unit->getPosition().y();
    double xto = xfrom + 30*unit->getVelocityX(); // 30, magic number
    double yto = yfrom + 30*unit->getVelocityY();
    double v_x = xto-xfrom;
    double v_y = yto-yfrom;
    Broodwar->drawLine(CoordinateType::Map, xfrom, yfrom, 
        (int)xto, (int)yto, Colors::White);
    Broodwar->drawTriangle(CoordinateType::Map, 
        (int)(xto - 0.1*v_y), (int)(yto + 0.1*v_x), 
        (int)(xto + 0.1*v_y), (int)(yto - 0.1*v_x), 
        (int)(xto + 0.1*v_x), (int)(yto + 0.1*v_y), 
        Colors::White); // 0.1, magic number
}

void BattleUnit::drawPath()
{
    for (std::vector<WalkTilePosition>::const_iterator it = _path.begin(); 
        it != _path.end(); ++it)
    {
        Broodwar->drawBox(CoordinateType::Map, it->getPosition().x() - 3, it->getPosition().y() - 3, 
            it->getPosition().x() + 3, it->getPosition().y() + 3, Colors::Yellow);
    }
}

void BattleUnit::drawBTPath()
{
    for (std::vector<TilePosition>::const_iterator it = _btpath.begin(); 
        it != _btpath.end(); ++it)
    {
        Broodwar->drawBox(CoordinateType::Map, it->x()*32 + 2, it->y()*32 + 2, 
            it->x()*32 + 30, it->y()*32 + 30, Colors::Yellow);
    }
}

void BattleUnit::drawPPath()
{
    for (std::vector<Position>::const_iterator it = _ppath.begin(); 
        it != _ppath.end(); ++it)
    {
        Broodwar->drawCircleMap(it->x(), it->y(), 3, Colors::Yellow);
    }
}

void BattleUnit::drawWalkability()
{
    MapManager* mapm = & MapManager::Instance();
    const int width = Broodwar->mapWidth();
    for (int x = 0; x < width*4; ++x)
    {
        for (int y = 0; y < Broodwar->mapHeight()*4; ++y)
        {
            if (mapm->walkability[x + y*width*4])
                Broodwar->drawBox(CoordinateType::Map, 8*x, 8*y, 8*x+6, 8*y+6, 
                Colors::Green);
            else
                Broodwar->drawBox(CoordinateType::Map, 8*x, 8*y, 8*x+6, 8*y+6, 
                Colors::Red);
        }
    }
}

void BattleUnit::drawEnclosingBox()
{
    Position p = unit->getPosition();
    Position top_left = Position(p.x() - unit->getType().dimensionLeft(), 
        p.y() - unit->getType().dimensionUp());
    Position bot_right = Position(p.x() + unit->getType().dimensionRight(), 
        p.y() + unit->getType().dimensionDown());
    Broodwar->drawBox(CoordinateType::Map, top_left.x(), top_left.y(), 
        bot_right.x(), bot_right.y(), Colors::Yellow);
}

void BattleUnit::drawTarget()
{
    Broodwar->drawCircle(CoordinateType::Map, target.x(), target.y(), 4, Colors::Purple, true);
}


/// TODO change this to use geometry (faster) and direct lines. 
/// We here consider only 8 possible directions and there are 16.
void BattleUnit::pathFind(std::vector<WalkTilePosition>& path, 
                          const Position& p_start, const Position& p_end)
{
    path.clear();
    MapManager* mapm = & MapManager::Instance();
    WalkTilePosition start(p_start);
    WalkTilePosition end(p_end);
    std::multimap<int, WalkTilePosition> openTiles;
    openTiles.insert(std::make_pair(0, start));
    std::map<WalkTilePosition, int> gmap;
    std::map<WalkTilePosition, WalkTilePosition> parent;
    std::set<WalkTilePosition> closedTiles;
    gmap[start]=0;
    parent[start]=start;
    while(!openTiles.empty())
    {
        WalkTilePosition p = openTiles.begin()->second;
        if (p==end)
        {
            std::vector<WalkTilePosition> reverse_path;
            while(p!=parent[p])
            {
                reverse_path.push_back(p);
                p=parent[p];
            }
            reverse_path.push_back(start);
            for(int i=reverse_path.size()-1; i>=0; i--)
                path.push_back(reverse_path[i]);
            return;
        }
        int fvalue = openTiles.begin()->first;
        int gvalue=gmap[p];
        openTiles.erase(openTiles.begin());
        closedTiles.insert(p);
        const int width = 4*BWAPI::Broodwar->mapWidth();
        int minx = max(p.x() - 1, 0);
        int maxx = min(p.x() + 1, width);
        int miny = max(p.y() - 1, 0);
        int maxy = min(p.y() + 1, 4*BWAPI::Broodwar->mapHeight());
        for(int x = minx; x <= maxx; x++)
            for(int y = miny; y <= maxy; y++)
            {
                if (!mapm->walkability[x + y*width]) continue;
                if (!(p.x() == x || p.y() == y) && 
                    !(mapm->walkability[p.x() + y*width] 
                        || mapm->walkability[x + p.y()*width])) continue;

                WalkTilePosition t(x,y);
                if (closedTiles.find(t)!=closedTiles.end()) continue;

                int g=gvalue+10;
                if (x != p.x() && y != p.y()) g+=4;
                int dx=abs(x-end.x());
                int dy=abs(y-end.y());
                int h=abs(dx-dy)*10+min(dx,dy)*14;
                int f=g+h;
                if (gmap.find(t)==gmap.end() || g<gmap.find(t)->second)
                {
                    gmap[t]=g;
                    std::pair<std::multimap<int, WalkTilePosition>::iterator, 
                        std::multimap<int, WalkTilePosition>::iterator> itp 
                        = openTiles.equal_range(f);
                    if (itp.second == itp.first) 
                        openTiles.insert(std::make_pair(f, t));
                    else {
                        for (std::multimap<int, WalkTilePosition>
                            ::const_iterator it = itp.first;
                            it != itp.second; ++it) 
                        {
                            if (it->second == t) 
                            {
                                openTiles.erase(it);
                                break;
                            } 
                        }
                        openTiles.insert(std::make_pair(f, t));
                    }
                    parent[t]=p;
                }
            }
    }
    std::vector<WalkTilePosition> nopath;
    path = nopath;
    return;
}

void BattleUnit::buildingsAwarePathFind(std::vector<TilePosition>& btpath, 
                          const TilePosition& start, const TilePosition& end)
{
    clock_t endTimer(0), startTimer(0) ;
    btpath.clear();
    MapManager* mapm = & MapManager::Instance();
    std::multimap<int, TilePosition> openTiles;
    openTiles.insert(std::make_pair(0, start));
    std::map<TilePosition, int> gmap;
    std::map<TilePosition, TilePosition> parent;
    std::set<TilePosition> closedTiles;
    gmap[start]=0;
    parent[start]=start;
    while(!openTiles.empty())
    {
        TilePosition p = openTiles.begin()->second;
        if (p==end)
        {
            std::vector<TilePosition> reverse_path;
            while(p!=parent[p])
            {
                reverse_path.push_back(p);
                p=parent[p];
            }
            reverse_path.push_back(start);
            for(int i=reverse_path.size()-1; i>=0; --i)
                btpath.push_back(reverse_path[i]);
            return;
        }
        //startTimer = clock();
        int fvalue = openTiles.begin()->first;
        int gvalue=gmap[p];
        openTiles.erase(openTiles.begin());
        closedTiles.insert(p);
        const int width = BWAPI::Broodwar->mapWidth();
        int minx = max(p.x() - 1, 0);
        int maxx = min(p.x() + 1, width - 1);
        int miny = max(p.y() - 1, 0);
        int maxy = min(p.y() + 1, BWAPI::Broodwar->mapHeight() - 1);
        //startTimer = clock() - startTimer;
        for(int x = minx; x <= maxx; x++)
            for(int y = miny; y <= maxy; y++)
            {
                if (!mapm->lowResWalkability[x + y*width]) continue;
                if (mapm->buildings[x + y*width]) continue;        // buildingsAware
                if (p.x() != x && p.y() != y && 
                    !mapm->lowResWalkability[p.x() + y*width] 
                    && !mapm->lowResWalkability[x + p.y()*width]
                    && mapm->buildings[p.x() + y*width]            // buildingsAware
                    && mapm->buildings[x + p.y()*width]) continue; // buildingsAware

                TilePosition t(x,y);
                if (closedTiles.find(t) != closedTiles.end()) continue;

                int g=gvalue+10;
                if (x != p.x() && y != p.y()) g+=4;
                int dx=abs(x-end.x());
                int dy=abs(y-end.y());
                int h=abs(dx-dy)*10+min(dx,dy)*14;
                int f=g+h;
                if (gmap.find(t)==gmap.end() || g<gmap.find(t)->second)
                {
                    gmap[t]=g;
                    std::pair<std::multimap<int, TilePosition>::iterator, 
                        std::multimap<int, TilePosition>::iterator> itp 
                        = openTiles.equal_range(f);
                    if (itp.second == itp.first) 
                        openTiles.insert(std::make_pair(f, t));
                    else 
                    {
                        for (std::multimap<int, TilePosition>
                            ::const_iterator it = itp.first;
                            it != itp.second; ++it) 
                        {
                            if (it->second == t) 
                            {
                                openTiles.erase(it);
                                break;
                            } 
                        }
                        openTiles.insert(std::make_pair(f, t));
                    }
                    parent[t]=p;
                }
            }
    endTimer += startTimer;
    }
    // empty path
    return;
}

void BattleUnit::straightLine(std::vector<TilePosition>& btpath, 
        const TilePosition& start, const TilePosition& end)
{

    // DO NOT CLEAR btpath for you
    if (start.x() == end.x() && start.y() == end.y()) return;
    TilePosition current = start;
    btpath.push_back(current);
    Position p_end = Position(end);
    while (Position(current).getDistance(p_end) > 31) // 31 for BuildTile Resolution
    {
        Vec line = Vec(end.x() - current.x(), 
            end.y() - current.y());
        if (line.y == 0.0)
        {
            int currentx = (line.x > 0.0 ? current.x() + 1 : current.x() - 1);
            current = TilePosition(currentx, current.y());
            btpath.push_back(current);
            continue;
        }
        double div = abs(line.x / line.y); // line.y != 0.0
        if (div > 2.0)
        {
            int currentx = (line.x > 0.0 ? current.x() + 1 : current.x() - 1);
            current = TilePosition(currentx, current.y());
        } 
        else if (div < 0.5)
        {
            int currenty = (line.y > 0.0 ? current.y() + 1 : current.y() - 1);
            current = TilePosition(current.x(), currenty);
        }
        else
        {
            int currentx = (line.x > 0.0 ? current.x() + 1 : current.x() - 1);
            int currenty = (line.y > 0.0 ? current.y() + 1 : current.y() - 1);
            current = TilePosition(currentx, currenty);
        }
        btpath.push_back(current);
    }
}

void BattleUnit::quickPathFind(std::vector<TilePosition>& btpath, 
                                        const TilePosition& start, 
                                        const TilePosition& end)
{
    clock_t endTimer(0), startTimer(0) ;
    btpath.clear();
    BWTA::Region* r_begin = BWTA::getRegion(start);
    BWTA::Region* r_end = BWTA::getRegion(end);
    if (!r_begin || !r_end) return; // defensive pgming w.r.t. BWTA
    if (r_begin == r_end)
    {
        btpath = BWTA::getShortestPath(start, end);
        return;
    }
    if (!r_end->isReachable(r_begin)) return;

    // only 2 regions
    std::set<BWTA::Chokepoint*> chokes = r_begin->getChokepoints();
    double dmin = 10000000000000000000000.0;
    TilePosition checkpoint;
    BWTA::Region* r_next;
    for (std::set<BWTA::Chokepoint*>::const_iterator it = chokes.begin();
        it != chokes.end(); ++it)
    {
        if (BWTA::getGroundDistance(TilePosition((*it)->getCenter()), end) < dmin) // to change?
        {
            checkpoint = TilePosition((*it)->getCenter());
            r_next = ((*it)->getRegions().first == r_begin ? 
                (*it)->getRegions().second : (*it)->getRegions().first);
        }
    }
    btpath = BWTA::getShortestPath(start, checkpoint);
    chokes = r_next->getChokepoints();
    dmin = 10000000000000000000000.0;
    TilePosition checkpoint2;
    for (std::set<BWTA::Chokepoint*>::const_iterator it = chokes.begin();
        it != chokes.end(); ++it)
    {
        if (BWTA::getGroundDistance(TilePosition((*it)->getCenter()), end) < dmin) // to change?
            checkpoint2 = TilePosition((*it)->getCenter());
    }
    std::vector<TilePosition> temp_path = BWTA::getShortestPath(checkpoint, checkpoint2);
    for (std::vector<TilePosition>::const_iterator it = temp_path.begin();
        it != temp_path.end(); ++it)
        btpath.push_back(*it);
    return;
}

/* Hack to do some computations only once every BU_EVAL_FREQ */
bool BattleUnit::tick()
{
    ++_tick;
    return !(_tick % BU_EVAL_FREQ);
}

void BattleUnit::update()
{
    drawVelocityArrow();
    if (tick())
    {
        //_path = BWTA::getShortestPath(unit->getTilePosition(), target);
        pathFind(_path, unit->getPosition(), target);
    }
    drawPath();
    // drawUnwalkable();
}

void BattleUnit::onUnitDestroy(Unit* u)
{
}

void BattleUnit::onUnitShow(Unit* u)
{
}

void BattleUnit::onUnitHide(Unit* u)
{
}

BWAPI::Unit* BattleUnit::findClosestEnemy(std::set<Unit*> &enemies)
{
    Unit* closest_enemy = NULL;
    for each(Unit* enemy in enemies)
    {
        if (closest_enemy)
        {
            if (unit->getDistance(closest_enemy) > unit->getDistance(enemy) && enemy->isVisible())
            {
                closest_enemy = enemy;
            }
        }
        else if (enemy->isVisible())
        {
            closest_enemy = enemy;
        }
    }
    return closest_enemy;
}