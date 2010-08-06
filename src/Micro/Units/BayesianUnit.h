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
    MODE_SCOUT
}; 

enum flock_value {
    FLOCK_NO,
    FLOCK_CONTACT,
    FLOCK_CLOSE,
    FLOCK_MEDIUM,
    FLOCK_FAR
};

enum inPos_value {
    INPOS_OK,
    INPOS_CONTACT,
    INPOS_CLOSE,
    INPOS_MEDIUM,
    INPOS_FAR
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

class BayesianUnit : public BattleUnit
{
protected:
    bool _ground_unit; // true when unit can move only on the ground
    std::vector<Vec> _dirv;
    int _maxDimension, _minDimension;
    double _maxDiag;
    BWAPI::Position _lastRightClick, _posAtMost13FramesAgo, _posAtMost23FramesAgo;
    bool _iThinkImBlocked;
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
    std::vector<std::vector<inPos_value> > _inPosValues;
    std::vector<std::vector<fightG_value> > _fightGValues;
    std::vector<std::vector<dodge_value> > _dodgeValues;
    std::vector<double> _flockProb; // TODO decide if static, perhaps unit dependant
    std::vector<double> _inPosProb; // TODO decide if static, perhaps unit dependant
    std::vector<double> _fightGProb; // TODO decide if static, perhaps unit dependant
    std::vector<double> _dodgeProb; // TODO decide if static, perhaps unit dependant
    UnitsGroup* _unitsGroup;
    std::multimap<double, BWAPI::Unit*> _rangeEnemies;
    std::map<occupation_type, double> _defaultProb;
    std::multimap<double, Vec> _dirvProb;

    inline void initDefaultProb();
    inline void computeFlockValues();
    inline void computeInPosValues();
    inline void computeFightGValues();
    void straightLine(std::vector<BWAPI::Position>& ppath, 
        const BWAPI::Position& p_start, 
        const BWAPI::Position& p_end, 
        bool quick=true);

    inline void updateDirV();
    inline void testIfBlocked();
    inline void updateRangeEnemies();
    inline void updateTargetEnemy();
    inline void setTargetEnemy(BWAPI::Unit* u);
    inline int computeDmg(BWAPI::Unit* u);
    void drawDirV();
    void updateObj();
    void drawObj(int number=0);
    void updateDir();
    void drawDir();
    inline void clickDir();
    void drawArrow(Vec& v);
    inline void updateAttractors();
    void drawAttractors();
    void drawFlockValues();
    void drawOccupation(int number);
    // TODO:
    // goal direction
    // range enemies
    inline double computeProb(unsigned int i);
    inline void computeProbs();
    inline void selectDir();
    void drawProbs(std::multimap<double, Vec>& probs, int number=0);
public:
    unit_mode _mode; // TODO : faudra le remettre en protected, c'est juste pour quelques tests
	void switchMode(unit_mode um);
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
    virtual bool canHit(BWAPI::Unit* enemy) = 0;
    virtual int damagesOn(BWAPI::Unit* enemy) = 0;
    virtual int getTimeToAttack() = 0;
    virtual bool withinRange(BWAPI::Unit* enemy) = 0;
    virtual BWAPI::UnitType* getListPriorite() = 0;
};