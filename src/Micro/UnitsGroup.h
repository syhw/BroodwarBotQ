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
#include "GoalManager.h"
#include <boost/bimap/bimap.hpp>

#define _UNITS_DEBUG 1

struct Dmg
{
    Dmg(int d, BWAPI::Unit* u) 
        : dmg(d)
        , unit(u)
    { }
    Dmg(const Dmg& d)
        : dmg(d.dmg)
        , unit(d.unit)
    { }
    int dmg;
    BWAPI::Unit* unit;
    bool operator<(const Dmg& d) const { return this->dmg > d.dmg || (this->dmg == d.dmg && this->unit < d.unit); }
    Dmg operator-=(int i) { this->dmg -= i; return *this; }
    Dmg operator+=(int i) { this->dmg += i; return *this; }
};

typedef boost::bimaps::bimap<BWAPI::Unit*, Dmg> UnitDmgBimap;
typedef UnitDmgBimap::relation UnitDmg;

class Formation;
class GoalManager;

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

    
	std::list<pGoal> goals; // list of goals to accomplish
    void displayTargets();  // debug purpose
public:
	pGoal getLastGoal();
	std::vector<pBayesianUnit> units;
    std::vector<BWAPI::TilePosition> btpath;
    std::set<BWAPI::Unit*> enemies;
    UnitDmgBimap unitDamages;
    pBayesianUnit leadingUnit;
	
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

#ifdef _UNITS_DEBUG
    void selectedUnits(std::set<pBayesianUnit>& u);
#endif
	const BayesianUnit& operator[](int i);
	void accomplishGoal();
	void switchMode(unit_mode um);
	void idle();
};

