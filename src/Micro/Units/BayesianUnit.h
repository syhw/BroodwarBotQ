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
    MODE_FIGHT_A    // air
}; 

enum flock_value {
    FLOCK_NO,
    FLOCK_CONTACT,
    FLOCK_CLOSE,
    FLOCK_MEDIUM,
    FLOCK_FAR
};

class BayesianUnit : public BattleUnit
{
protected:
    bool _ground_unit; // true when unit can move only on the ground
    unit_mode _mode;
    int _sheight, _slarge;
    std::vector<Vec> _dirv;
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
    std::vector<double> _flockProb; // TODO decide if static, perhaps unit dependant
    UnitsGroup* _unitsGroup;
    std::multimap<double, BWAPI::Unit*> _rangeEnemies;
    std::map<occupation_type, double> _defaultProb;
    BWAPI::Unit* oldTarget;

    inline void initDefaultProb();
    inline void computeFlockValues();
    void straightLine(std::vector<BWAPI::Position>& ppath, 
        const BWAPI::Position& p_start, 
        const BWAPI::Position& p_end, 
        bool quick=true);
    void switchMode(unit_mode um);
    inline void updateDirV();
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
    void drawProbs(std::multimap<double, Vec>& probs, int number=0);
    inline void deleteRangeEnemiesElem(BWAPI::Unit* u);
    inline void updateRangeEnemiesWith(BWAPI::Unit* u);
public:
    Vec dir, obj; // dir=current direction, obj=pathfinder's direction
    // std::map<attractor_type, std::vector<BWAPI::Position>> prox; 
    BayesianUnit(BWAPI::Unit* u, UnitsGroup* ug);
    ~BayesianUnit();
    virtual void onUnitDestroy(BWAPI::Unit* u);
    virtual void onUnitShow(BWAPI::Unit* u);
    virtual void onUnitHide(BWAPI::Unit* u);
    void update();
    virtual void attackMove(const BWAPI::Position& p);
    std::multimap<double, BWAPI::Unit*>& getRangeEnemies();
    BWAPI::Unit* getOldTarget();
    void setOldTarget(BWAPI::Unit* newTarget);

    void attackEnemy(BWAPI::Unit* u, BWAPI::Color col);
    virtual void micro() = 0;
    virtual bool canHit(BWAPI::Unit* enemy) = 0;
    virtual int damagesOn(BWAPI::Unit* enemy) = 0;
    virtual int getTimeToAttack() = 0;
    virtual bool withinRange(BWAPI::Unit* enemy) = 0;
    virtual BWAPI::UnitType* getListPriorite() = 0;
};