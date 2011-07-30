#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <algorithm>
#include "Regions/WalkTilePosition.h"


class BattleUnit
{
    friend void baseMicro();
    friend void zealotMicro();
protected:
    int _accel;
    double _topSpeed;
    BWAPI::TilePosition _tptarget;
#ifndef __RELEASE_OPTIM__
    void drawVelocityArrow() const;
    void drawPath() const;
    void drawBTPath() const;
    void drawPPath() const;
    void drawWalkability() const;
    void drawEnclosingBox() const;
    void drawTarget() const;
#endif
    BWAPI::Position _unitPos;
    std::vector<BWAPI::TilePosition> _btpath;
    std::vector<WalkTilePosition> _path;
    std::vector<BWAPI::Position> _ppath;
    static void pathFind(std::vector<WalkTilePosition>& path, 
        const BWAPI::Position& p_start, const BWAPI::Position& p_end);
    static void straightLine(std::vector<BWAPI::TilePosition>& btpath, 
        const BWAPI::TilePosition& start, const BWAPI::TilePosition& end);
    BWAPI::Unit* oldTarget;

public:
	int _sheight, _slarge;
    static void buildingsAwarePathFind(std::vector<BWAPI::TilePosition>& btpath, 
        const BWAPI::TilePosition& start, const BWAPI::TilePosition& end);
    static void damagesAwarePathFind(std::vector<BWAPI::TilePosition>& btpath, 
        const BWAPI::TilePosition& start, const BWAPI::TilePosition& end, int* tab);
    static void quickPathFind(std::vector<BWAPI::TilePosition>& btpath, // works only with correct Regions
        const BWAPI::TilePosition& start,                               // often not the case on custom maps
        const BWAPI::TilePosition& end);
    unsigned int _tick;
	BWAPI::Unit* unit;
    BWAPI::Unit* targetEnemy; // public for debug purpose, __USE__ setTargetEnemy(Unit*)
    BWAPI::Unit* oorTargetEnemy;
	BWAPI::Position target; // display debug purpose
    const std::vector<BWAPI::Position> & getPPath();
    BWAPI::Position BattleUnit::getPPath(unsigned int n);

	BattleUnit(BWAPI::Unit* unit);
    ~BattleUnit();
    void attack(const BWAPI::Position& p);
    bool operator == (const BattleUnit& bu) const;
    void update() = 0;
    virtual void onUnitDestroy(BWAPI::Unit* u);
    virtual void onUnitShow(BWAPI::Unit* u);
    virtual void onUnitHide(BWAPI::Unit* u);
};
