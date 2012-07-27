#include <PrecompiledHeader.h>
#include "BattleUnit.h"
#include "Utils/Vec.h"
#include "Defines.h"

using namespace BWAPI;

//////////////////////////////////////////////////
////////            BattleUnit             ///////
//////////////////////////////////////////////////

BattleUnit::BattleUnit(Unit* unit)
: unit(unit)
, _tick(0)
, targetEnemy(NULL)
, oorTargetEnemy(NULL)
, target(unit->getPosition())
, _tptarget(unit->getTilePosition())
, _sheight(unit->getType().dimensionUp() + unit->getType().dimensionDown())
, _slarge(unit->getType().dimensionRight() + unit->getType().dimensionLeft())
, _accel(unit->getType().acceleration())
, _topSpeed(unit->getType().topSpeed())
{
}

BattleUnit::~BattleUnit()
{
}

void BattleUnit::attack(const Position& p)
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
	unit->attack(p);
}

bool BattleUnit::operator == (const BattleUnit& bu) const
{
	return (unit == bu.unit) && (target == bu.target);
}


const std::vector<Position> & BattleUnit::getPPath()
{
    return _ppath;
}

Position BattleUnit::getPPath(unsigned int n)
{
    return _ppath[n];
}

#ifdef __MICRO_DEBUG__
void BattleUnit::drawVelocityArrow() const
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

void BattleUnit::drawPath() const
{
    if (_path.empty())
        return;
    for (std::vector<WalkTilePosition>::const_iterator it = _path.begin(); 
        it != _path.end(); ++it)
    {
        Broodwar->drawBox(CoordinateType::Map, it->getPosition().x() - 3, it->getPosition().y() - 3, 
            it->getPosition().x() + 3, it->getPosition().y() + 3, Colors::Yellow);
    }
}

void BattleUnit::drawBTPath() const
{
    if (btpath.empty())
        return;
    for (std::vector<TilePosition>::const_iterator it = btpath.begin(); 
        it != btpath.end(); ++it)
    {
        Broodwar->drawBox(CoordinateType::Map, it->x()*32 + 2, it->y()*32 + 2, 
            it->x()*32 + 30, it->y()*32 + 30, Colors::Yellow);
    }
}

void BattleUnit::drawPPath() const
{
    if (_ppath.empty())
        return;
    for (std::vector<Position>::const_iterator it = _ppath.begin(); 
        it != _ppath.end(); ++it)
    {
        Broodwar->drawCircleMap(it->x(), it->y(), 3, Colors::Yellow);
    }
}

void BattleUnit::drawEnclosingBox() const
{
    Position p = unit->getPosition();
    Position top_left = Position(p.x() - unit->getType().dimensionLeft(), 
        p.y() - unit->getType().dimensionUp());
    Position bot_right = Position(p.x() + unit->getType().dimensionRight(), 
        p.y() + unit->getType().dimensionDown());
    Broodwar->drawBox(CoordinateType::Map, top_left.x(), top_left.y(), 
        bot_right.x(), bot_right.y(), Colors::Yellow);
}

void BattleUnit::drawTarget() const
{
    Broodwar->drawCircleMap(target.x(), target.y(), 4, Colors::Purple, true);
    Broodwar->drawLineMap(target.x(), target.y(), unit->getPosition().x(), unit->getPosition().y(), Colors::Purple);
}
#endif