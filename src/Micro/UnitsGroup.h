#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <math.h>
#include "BayesianUnit.h"
#include "Goal.h"
#include "Formations.h"
#include <Vec.h>
#include <set>
#include <boost/bimap/bimap.hpp>
#include <EUnitsFilter.h>

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
    bool operator<(const Dmg& d) const { return ((this->hp - this->dmg) < (d.hp + d.dmg) || (this->hp + this->dmg == d.hp + d.dmg) && this->unit < d.unit); }
    //bool operator<(const Dmg& d) const { return (this->dmg > d.dmg || this->dmg == d.dmg && this->unit < d.unit); }
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

class UnitsGroup
{
private:
	int totalHP;
	int totalPower;
	std::list<pGoal> goals;
    EUnitsFilter* _eUnitsFilter;
    std::set<BWAPI::Unit*> _mergersHT;
    inline void updateNearbyEnemyUnitsFromFilter(BWAPI::Position p, double radius = 400.0);
    void displayTargets();  // debug purpose
public:
	bool isWaiting(); //Return if the 1st goal is accomplished && no other goals
	pGoal getLastGoal();
	std::vector<pBayesianUnit> units;
    //std::vector<BWAPI::TilePosition> btpath;
    std::vector<BWAPI::Position> ppath;
    std::map<BWAPI::Unit*, BWAPI::Position> enemies;
    UnitDmgBimap unitDamages;
    pBayesianUnit leadingUnit;
    BWAPI::Unit* defaultTargetEnemy;
	
	UnitsGroup();
	virtual ~UnitsGroup();

    BWAPI::Position center;
    double stdDevRadius, maxRadius;
    Vec centerSpeed;
	std::map<BWAPI::UnitSizeType, int> sizes;

	virtual void update();
	virtual void display();
	// Goals interface
	virtual void attackMove(int x, int y);
	virtual void attackMove(BWAPI::Position& p);
	virtual void move(BWAPI::Position& p);

	virtual void formation(pFormation f);
	virtual void setGoals(std::list<pGoal>& goals);
	virtual void addGoal(pGoal goal);
	
	inline void updateCenter();
    virtual BWAPI::Position getCenter() const;
    inline double getDistance(BWAPI::Unit* u) const;

    // BWAPI interface
    virtual void onUnitDestroy(BWAPI::Unit* u);
    virtual void onUnitShow(BWAPI::Unit* u);
    virtual void onUnitHide(BWAPI::Unit* u);

    // Units interface
    virtual void takeControl(BWAPI::Unit* u);
    virtual void giveUpControl(BWAPI::Unit* u);
	bool emptyUnits();
	bool emptyGoals();
    int size() const;
    int getTotalHP() const;
    std::vector<pBayesianUnit>* getUnits();

    inline void templarMergingStuff();
    void signalMerge(BWAPI::Unit* u);

#ifdef _UNITS_DEBUG
    void selectedUnits(std::set<pBayesianUnit>& u);
#endif
	const BayesianUnit& operator[](int i);
	void accomplishGoal();
	void switchMode(unit_mode um);
	void idle();
};

