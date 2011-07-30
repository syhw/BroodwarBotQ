#pragma once
#include <PrecompiledHeader.h>
#include <BWAPI.h>
#include <BWTA.h>
#include <math.h>
#include "Utils/Vec.h"
#include "BattleUnit.h"
#include "Regions/MapManager.h"
//#include "Micro/UnitsGroup.h"
#include <map>
#include <set>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>

class BayesianUnit;
typedef boost::shared_ptr<BayesianUnit> pBayesianUnit;

// #define PROBT 1
// #define WITH_FLOCKING 1
// #define __HEIGHTS_ATTRACTION__

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
    MODE_FIGHT_G,   // ground
    MODE_FIGHT_A,   // air
    MODE_SCOUT,
#ifdef WITH_FLOCKING
    MODE_FLOCK,
#endif
    MODE_MOVE
}; 

enum repulse_value {
    REPULSE_NO,
    REPULSE_LOW,
    REPULSE_HIGH
};

enum damage_value {
    DAMAGE_NO,
    DAMAGE_LOW,
    DAMAGE_MED,
    DAMAGE_HIGH
};

struct ProbTablesData
{
	friend class boost::serialization::access;
#ifdef __LEARNING_PROB_TABLES__
	int version;
	int score;
	void fillProbTables();
#endif
    std::vector<double> _damageProb;
    std::vector<double> _repulseProb;
    std::map<int, double> _defaultProb; // int == occupation_type, for s13n
#ifdef __HEIGHTS_ATTRACTION__
    std::vector<double> _heightProb;
#endif
	ProbTablesData();
	ProbTablesData(int utID, std::vector<double>damageP, 
		std::vector<double>repulseP
		, std::map<int, double> defaultP // int == occupation_type, for s13n
#ifdef __HEIGHTS_ATTRACTION__
		, std::vector<double> heightP
#endif
		);
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

BOOST_CLASS_TRACKING(ProbTablesData, boost::serialization::track_never)

struct ProbTables
{
	int unitTypeID; // -1 for ground, -2 for flying, -3 for special
	ProbTablesData probTablesData;
	ProbTables(int utID);
	~ProbTables();
};

class BayesianUnit : public BattleUnit
{
protected:
    HANDLE _pathMutex;
    static DWORD WINAPI StaticLaunchPathfinding(void* obj);
    DWORD LaunchPathfinding();
    std::vector<Vec> _dirv;
    int _maxDimension, _minDimension;
    int _lastAttackFrame;
    int _lastMoveFrame;
    int _lastClickFrame;
    double _maxDiag;
    int _maxWeaponsRange;
    BWAPI::Position _lastRightClick, _posAtMost13FramesAgo, _posAtMost23FramesAgo;
    bool _iThinkImBlocked;
    int _lastTotalHP;
    std::list<int> _HPLosts;
    int _sumLostHP;
    const int _refreshPathFramerate; // should be static TODO
    int _maxDistWhileRefreshingPath; // not static because dependent on the speed
    bool _newPath;
    Position _inPos;
    bool _fleeing;
    bool _fightMoving;
    int _fleeingDmg;
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
    std::vector<repulse_value> _repulseValues;
#ifdef WITH_FLOCKING
    static std::vector<repulse_value> _flockValues;
#endif
    std::vector<damage_value> _damageValues;
	const ProbTables* _probTables;
    UnitsGroup* _unitsGroup;
    std::multimap<double, BWAPI::Unit*> _rangeEnemies;
    std::multimap<double, Vec> _dirvProb;

    inline void computeRepulseValues();
#ifdef WITH_FLOCKING
    inline void computeFlockValues();
#endif
    inline void computeDamageValues();
    void straightLine(std::vector<BWAPI::Position>& ppath, 
        const BWAPI::Position& p_start, 
        const BWAPI::Position& p_end, 
        bool quick=true);

    inline void updateDirV();
    inline void testIfBlocked();
    inline void resumeFromBlocked();
    void updateRangeEnemies();
    void clearDamages();
    void updateTargetEnemy();
    inline void setTargetEnemy(BWAPI::Unit* u);
    int computeDmg(BWAPI::Unit* u);
    bool inRange(BWAPI::Unit* u);
    bool outRanges(BWAPI::Unit* u);
    void drawDirV();
    void updateObj();
    void drawObj(int number=0);
    void updateDir();
    void drawDir();
    inline void clickDir();
    inline void clickScout();
    inline void clickTarget();
    void flee();
    int fightMove();
    void drawArrow(Vec& v);
    inline void updateAttractors();
    void drawAttractors();
    void drawRepulseValues();
#ifdef WITH_FLOCKING
    void drawFlockValues();
#endif
    void drawOccupation(int number);
    // TODO:
    // goal direction
    // range enemies
    inline double computeProb(unsigned int i);
    inline void computeProbs();
    inline void selectDir(const Vec& criterium);
    virtual int addRangeGround();
    virtual int addRangeAir();
    virtual bool decideToFlee(); // sets _fleeing
    void attackEnemyUnit(BWAPI::Unit* u);
    void simpleFlee();
    bool dodgeStorm();
    bool dragScarab();
    bool dragMine();
    void drawProbs(std::multimap<double, Vec>& probs, int number=0);
    unit_mode _mode;
    std::set<Unit*> _targetingMe;
    void updateTargetingMe();
public:
	static pBayesianUnit newBayesianUnit(BWAPI::Unit* u);

    void move(BWAPI::Position p); // debug purposes
	void switchMode(unit_mode um);
	void setUnitsGroup(UnitsGroup* ug);
    unit_mode getMode();
    int getMaxDimension();
    void updatePPath();
    Vec dir, obj; // dir=current direction, obj=pathfinder's direction
    BayesianUnit(BWAPI::Unit* u, const ProbTables* probTables);
    ~BayesianUnit();
    BWAPI::UnitType getType();

    void update();
    virtual void attack(const BWAPI::Position& p);

    virtual void micro() = 0;
    virtual void check() = 0;
    virtual int getAttackDuration() = 0;
    virtual std::set<BWAPI::UnitType> getSetPrio() = 0;
};
