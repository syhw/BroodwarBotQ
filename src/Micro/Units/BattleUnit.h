#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <algorithm>
#include "WalkTilePosition.h"

#define BU_EVAL_FREQ 6
#define UNIT_DEBUG 1

class BattleUnit
{
    friend void baseMicro();
    friend void zealotMicro();
protected:
    int _sheight, _slarge;
    int _accel;
    double _topSpeed;
#ifdef UNIT_DEBUG
    std::string _unitType;
#endif
    void drawVelocityArrow();
    void drawPath();
    void drawBTPath();
    void drawPPath();
    void drawWalkability();
    void drawEnclosingBox();
    void drawTarget();
    BWAPI::Position _unitPos;
    std::vector<BWAPI::TilePosition> _btpath;
    std::vector<WalkTilePosition> _path;
    std::vector<BWAPI::Position> _ppath;
    static void pathFind(std::vector<WalkTilePosition>& path, 
        const BWAPI::Position& p_start, const BWAPI::Position& p_end);
    static void straightLine(std::vector<BWAPI::TilePosition>& btpath, 
        const BWAPI::TilePosition& start, const BWAPI::TilePosition& end);
    bool inline tick();
    BWAPI::Unit* oldTarget;

public:
    static void buildingsAwarePathFind(std::vector<BWAPI::TilePosition>& btpath, 
        const BWAPI::TilePosition& start, const BWAPI::TilePosition& end);
    static void quickPathFind(std::vector<BWAPI::TilePosition>& btpath, // works only with correct Regions
        const BWAPI::TilePosition& start,                               // often not the case on custom maps
        const BWAPI::TilePosition& end);
    unsigned int _tick;
	BWAPI::Unit* unit;
    BWAPI::Unit* targetEnemy; // public for debug purpose, __USE__ setTargetEnemy(Unit*)
	BWAPI::Position target; // display debug purpose
	int timeIdle; // hack to remove soon... evite le conflit avec la reussite d'objectif dans le test UnitsGroup::keepDistance().
	              // Cette variable sera inutile avec le flocking.
	              // Ou avec un Timer (TimeManager soon ready !)

	BattleUnit(BWAPI::Unit* unit);
    ~BattleUnit();
    void attackMove(const BWAPI::Position& p);
    bool operator == (const BattleUnit& bu) const;
    void update();
    virtual void onUnitDestroy(BWAPI::Unit* u);
    virtual void onUnitShow(BWAPI::Unit* u);
    virtual void onUnitHide(BWAPI::Unit* u);

    BWAPI::Unit* findClosestEnemy(std::set<BWAPI::Unit*> &enemies); // Function to remove ?
    virtual int damagesOn(BWAPI::Unit* enemy) = 0;
    virtual bool withinRange(BWAPI::Unit* enemy) = 0;
};
