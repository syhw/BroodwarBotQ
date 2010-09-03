#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <math.h>
#include "Vec.h"
#include "BattleUnit.h"
#include "MapManager.h"
//#include "UnitsGroup.h"
#include <map>
#include <set>
#include <boost/shared_ptr.hpp>

class BayesianUnit;
typedef boost::shared_ptr<BayesianUnit> pBayesianUnit;

#include <windows.h>
#include <process.h>

// #define PROBT 1

#define NUMBER_OF_PRIORITY 5
class BayesianUnit;
typedef boost::shared_ptr<BayesianUnit> pBayesianUnit;


// TODO, this class has to be derived to take Flying/Ground/Special Units 
// (templars, tanks, lurkers, etc.) into account

class UnitsGroup;

enum occupation_type {
    OCCUP_NO,
    OCCUP_EUNIT,
    OCCUP_UNIT,
    OCCUP_BLOCKING,
    OCCUP_BUILDING
};

enum unit_mode {
    MODE_INPOS,
    MODE_FLOCK,
    MODE_FLOCKFORM, // flock in formation
    MODE_FIGHT_G,   // ground
    MODE_FIGHT_A,    // air
    MODE_SCOUT,
    MODE_MOVE
}; 

enum flock_value {
    FLOCK_NO,
    FLOCK_CONTACT,
    FLOCK_CLOSE,
    FLOCK_MEDIUM,
    FLOCK_FAR
};

enum repulse_value {
    REPULSE_NO,
    REPULSE_LOW,
    REPULSE_HIGH
};

enum fightG_value { 
    FIGHTG_NO,
    FIGHTG_LIGHT,
    FIGHTG_MEDIUM,
    FIGHTG_HEAVY,
    FIGHTG_DEAD
};

enum dodge_value {
    DODGE_NO,
    DODGE_CONTACT,
    DODGE_CLOSE,
    DODGE_MEDIUM
};

enum damage_value {
    DAMAGE_NO,
    DAMAGE_LOW,
    DAMAGE_MED,
    DAMAGE_HIGH
};

class BayesianUnit : public BattleUnit
{
protected:
    HANDLE _pathMutex;
    static DWORD WINAPI StaticLaunchPathfinding(void* obj);
    DWORD LaunchPathfinding();
    bool _ground_unit; // true when unit can move only on the ground
    std::vector<Vec> _dirv;
    int _maxDimension, _minDimension;
    int _lastAttackOrder;
    int _lastClickFrame;
    double _maxDiag;
    BWAPI::Position _lastRightClick, _posAtMost13FramesAgo, _posAtMost23FramesAgo;
    bool _iThinkImBlocked;
    int _lastTotalHP;
    const int _refreshPathFramerate;
    int _maxDistWhileRefreshingPath;
    bool _newPath;
    int _attackDuration;
    Position _inPos;
    bool _fleeing;
    //std::multimap<BWAPI::Position, attractor_type> _prox;
    std::vector<occupation_type> _occupation;
    // dirv[attractor] = direction relative to an attractor
    /* Our Unit is in 12. Here are the indices of the Vec()s.
    ________________
    | 0| 5|10|15|20|
    | 1| 6|11|16|21|
    | 2| 7|12|17|22|
    | 3| 8|13|18|23|
    | 4| 9|14|19|24|
    ----------------
    This grid because there are only 16 possible directions in Broodwar */
    MapManager* mapManager;
    std::vector<std::vector<flock_value> > _flockValues; // one vector<flock_value> per unit with which we flock
    std::vector<repulse_value> _repulseValues;
    std::vector<damage_value> _damageValues;
    std::vector<double> _flockProb; // TODO decide if static, perhaps unit dependant
    std::vector<double> _damageProb; // TODO decide if static, perhaps unit dependant
    std::vector<double> _repulseProb;
    std::vector<double> _heightProb;
    UnitsGroup* _unitsGroup;
    std::multimap<double, BWAPI::Unit*> _rangeEnemies;
    std::map<occupation_type, double> _defaultProb;
    std::multimap<double, Vec> _dirvProb;

    inline void initDefaultProb();
    inline void computeFlockValues();
    inline void computeRepulseValues();
    inline void computeDamageValues();
    void straightLine(std::vector<BWAPI::Position>& ppath, 
        const BWAPI::Position& p_start, 
        const BWAPI::Position& p_end, 
        bool quick=true);

    inline void updateDirV();
    inline void testIfBlocked();
    inline void resumeFromBlocked();
    void updateRangeEnemies();
    void updateTargetEnemy();
    inline void setTargetEnemy(BWAPI::Unit* u);
    inline int computeDmg(BWAPI::Unit* u);
    void drawDirV();
    void updateObj();
    void drawObj(int number=0);
    void updateDir();
    void drawDir();
    inline void clickDir();
    void flee();
    void fightMove();
    void drawArrow(Vec& v);
    inline void updateAttractors();
    void drawAttractors();
    void drawFlockValues();
    void drawRepulseValues();
    void drawOccupation(int number);
    // TODO:
    // goal direction
    // range enemies
    inline double computeProb(unsigned int i);
    inline void computeProbs();
    inline void selectDir(const Vec& criterium);
    virtual int addRangeGround();
    virtual int addRangeAir();
    void drawProbs(std::multimap<double, Vec>& probs, int number=0);
    unit_mode _mode;
public:
	void switchMode(unit_mode um);
    int getMaxDimension();
    Vec dir, obj; // dir=current direction, obj=pathfinder's direction
    BayesianUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~BayesianUnit();

    virtual void onUnitDestroy(BWAPI::Unit* u);
    virtual void onUnitShow(BWAPI::Unit* u);
    virtual void onUnitHide(BWAPI::Unit* u);

    void update();
    virtual void attackMove(const BWAPI::Position& p);

    void attackEnemy(BWAPI::Unit* u, BWAPI::Color col);
    virtual void micro() = 0;
    virtual void check() = 0;
    virtual bool canHit(BWAPI::Unit* enemy) = 0;
    virtual int damagesOn(BWAPI::Unit* enemy) = 0;
    virtual int getTimeToAttack() = 0;
    virtual bool withinRange(BWAPI::Unit* enemy) = 0;
    virtual std::set<BWAPI::UnitType> getSetPrio() = 0;
};
