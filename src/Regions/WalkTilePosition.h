#pragma once
#include <BWAPI.h>
#include <math.h>

class WalkTilePosition
{
    public:
        WalkTilePosition()
            : _x(0), _y(0)
        { 
        }
        WalkTilePosition(const BWAPI::Position& position) 
            : _x(position.x() / 8), _y(position.y() / 8) // 8 pixels for a WalkTile
            //: _x((position.x() + 4)/ 8), _y((position.y()+4) / 8) // 8 pixels for a WalkTile
        {
        }
        WalkTilePosition(const BWAPI::TilePosition& tile)
            : _x(tile.x() * 4), _y(tile.y() * 4)
        {
        }
        WalkTilePosition(int x, int y) 
            : _x(x), _y(y)
        {
        }
        ~WalkTilePosition() { }
        bool operator == (const WalkTilePosition& walkTilePosition) const 
        {
            return (_x == walkTilePosition.x() && _y == walkTilePosition.y());
        }
        bool operator != (const WalkTilePosition& walkTilePosition) const
        {
            return (_x != walkTilePosition.x() || _y != walkTilePosition.y());
        }
        bool operator < (const WalkTilePosition& walkTilePosition) const
        {
            return (_x < walkTilePosition.x() ||
                   (_x == walkTilePosition.x() && _y < walkTilePosition.y()));
        }
        WalkTilePosition operator+ (const WalkTilePosition& position) const
        {
            return WalkTilePosition(_x + position.x(), _y + position.y());
        }
        WalkTilePosition operator- (const WalkTilePosition& position) const
        {
            return WalkTilePosition(_x - position.x(), _y - position.y());
        }
        WalkTilePosition operator+= (const WalkTilePosition& position)
        {
            _x += position.x();
            _y += position.y();
        }
        WalkTilePosition operator-= (const WalkTilePosition& position)
        {
            _x -= position.x();
            _y -= position.y();
        }
        double getDistance(const WalkTilePosition& position) const
        {
            double xx = _x - position.x();
            double yy = _y - position.y();
            return sqrt(xx*xx + yy*yy);
        }
        BWAPI::Position getPosition() const
        {
            return BWAPI::Position(8*_x + 4, 8*_y + 4);
        }
        // double getLength() const;
        int x() const
        {
            return _x;
        }
        int y() const
        {
            return _y;
        }
    private:
        int _x;
        int _y;
};

