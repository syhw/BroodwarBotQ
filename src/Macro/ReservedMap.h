#pragma once
#include <RectangleArray.h>
#include <BWAPI.h>
class ReservedMap
{
  public:
    static ReservedMap* create();
    static void destroy();
    void reserveTiles(BWAPI::TilePosition position, BWAPI::UnitType type, int width = 1, int height = 1);
    void freeTiles(BWAPI::TilePosition position, int width = 1, int height = 1);
    bool isReserved(int x, int y);
    bool isReserved(BWAPI::TilePosition p);
    BWAPI::UnitType getReservedType(int x, int y);
    BWAPI::UnitType getReservedType(BWAPI::TilePosition p);
  private:
    ReservedMap();
    ~ReservedMap();
    Util::RectangleArray<BWAPI::UnitType> reserveMap;
};
extern ReservedMap* TheReservedMap;