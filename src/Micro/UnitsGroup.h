#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <math.h>
#include "Micro/Units/BayesianUnit.h"
#include "Micro/Goals/Goal.h"
#include "Micro/Formations.h"
#include "Utils/Vec.h"
#include <set>
#include <boost/bimap/bimap.hpp>
#include "Intelligence/EUnitsFilter.h"

#define _UNITS_DEBUG 1

struct Dmg
{
    Dmg(int d, BWAPI::Unit* u, int h) 
        : dmg(d)
        , unit(u)
        , hp(h)
    { }
    Dmg(const Dmg& d)
        : dmg(d.dmg)
        , unit(d.unit)
        , hp(d.hp)
    { }
    int dmg;
    int hp;
    BWAPI::Unit* unit;
    bool operator<(const Dmg& d) const { return ((this->hp - this->dmg) < (d.hp - d.dmg) || (this->hp - this->dmg == d.hp - d.dmg) && this->unit < d.unit); }
    Dmg operator-=(int i) { this->dmg -= i; return *this; }
    Dmg operator+=(int i) { this->dmg += i; return *this; }
};

typedef boost::bimaps::bimap<BWAPI::Unit*, Dmg> UnitDmgBimap;
typedef UnitDmgBimap::relation UnitDmg;

class Formation;

struct i_dist 
{
	unsigned int ind;
	double dist;
	i_dist(unsigned int i, double d): ind(i), dist(d) {}
	//bool operator<(i_dist& ext) { return (ext.dist < dist); }
};

class BasicUnitsGroup
{
protected:
	std::list<pGoal> goals;
public:
	std::vector<pBayesianUnit> units;
	virtual void update();
    // Units interface
    bool removeUnit(BWAPI::Unit* u);
	// Goals interface
	void accomplishGoal();
	bool emptyUnits();
	bool emptyGoals();
    int size() const;
};

class UnitsGroup : public BasicUnitsGroup
{
private:
	int totalHP;
    int totalMinPrice;
    int totalGazPrice;
    int totalSupply;
    EUnitsFilter* _eUnitsFilter;
    std::set<BWAPI::Unit*> _mergersHT;
    bool _hasDetection;
    inline void updateNearbyEnemyUnitsFromFilter(BWAPI::Position p, double radius = 400.0);
    double evaluateForces();
    void displayTargets();  // debug purpose
public:
	inline void dispatchCompleteUnit(pBayesianUnit bu);
    void takeControl(BWAPI::Unit* u);
    void giveUpControl(BWAPI::Unit* u);
	bool isWaiting(); //Return if the 1st goal is accomplished && no other goals
	pGoal getLastGoal();
    std::list<pBayesianUnit> arrivingUnits;
    std::vector<BWAPI::Position> ppath;
    std::map<BWAPI::Unit*, BWAPI::Position> enemies;
    BWAPI::Position enemiesCenter;
    int enemiesAltitude;
    UnitDmgBimap unitDamages;
    pBayesianUnit leadingUnit;
    BWAPI::Unit* defaultTargetEnemy;
	
	UnitsGroup();
	virtual ~UnitsGroup();

    BWTA::Chokepoint* nearestChoke;
    double distToNearestChoke;
    BWAPI::Position center;
    int groupAltitude;
    double stdDevRadius, maxRadius;
    Vec centerSpeed;
	std::map<BWAPI::UnitSizeType, int> sizes;

	virtual void update();
	virtual void display();

	// Units interface
    pBayesianUnit addUnit(BWAPI::Unit* u);

	// Goals interface
	void setGoals(std::list<pGoal>& goals);
	void addGoal(pGoal goal);
    void addGoalFront(pGoal goal);

	virtual void attack(int x, int y);
	virtual void attack(BWAPI::Position& p);
	virtual void move(BWAPI::Position& p);

	virtual void formation(pFormation f);
	
	inline void updateCenter();
    virtual BWAPI::Position getCenter() const;
    inline double getDistance(BWAPI::Unit* u) const;

    // BWAPI interface
    virtual void onUnitDestroy(BWAPI::Unit* u);
    virtual void onUnitShow(BWAPI::Unit* u);
    virtual void onUnitHide(BWAPI::Unit* u);

	inline void updateGroupStrengh(BWAPI::Unit* u);
    int getTotalHP() const;
    std::vector<pBayesianUnit>* getUnits();

    inline void templarMergingStuff();
    void signalMerge(BWAPI::Unit* u);

#ifdef _UNITS_DEBUG
    void selectedUnits(std::set<pBayesianUnit>& u);
#endif
	const BayesianUnit& operator[](ptrdiff_t i);
	void switchMode(unit_mode um);
	void idle();
};

