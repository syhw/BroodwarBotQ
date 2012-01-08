#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <algorithm>
#include "Regions/WalkTilePosition.h"
#include "Defines.h"


class BattleUnit
{
    friend void baseMicro();
    friend void zealotMicro();
protected:
    int _accel;
    double _topSpeed;
    BWAPI::TilePosition _tptarget;
#ifdef __MICRO_DEBUG__
    void drawVelocityArrow() const;
    void drawPath() const;
    void drawBTPath() const;
    void drawPPath() const;
    void drawWalkability() const;
    void drawEnclosingBox() const;
    void drawTarget() const;
#endif
    BWAPI::Position _unitPos;
    std::vector<WalkTilePosition> _path;
    std::vector<BWAPI::Position> _ppath;
    BWAPI::Unit* oldTarget;

public:
    std::vector<BWAPI::TilePosition> btpath;
	int _sheight, _slarge;
    unsigned int _tick;
	BWAPI::Unit* unit;
    BWAPI::Unit* targetEnemy; // public for debug purpose, __USE__ setTargetEnemy(Unit*)
    BWAPI::Unit* oorTargetEnemy;
	BWAPI::Position target; // public for display debug purpose
    const std::vector<BWAPI::Position> & getPPath();
    BWAPI::Position BattleUnit::getPPath(unsigned int n);

	BattleUnit(BWAPI::Unit* unit);
    virtual ~BattleUnit();
    void attack(const BWAPI::Position& p);
    bool operator == (const BattleUnit& bu) const;
    virtual void update() = 0;
};
