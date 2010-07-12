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