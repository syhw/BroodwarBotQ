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
#include "MicroManager.h"
#include "GoalManager.h"

#define _UNITS_DEBUG 1

//class Goal;
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
    std::vector<pBayesianUnit> units;
    
	std::list<pGoal> goals; // list of goals to accomplish
    void displayTargets();  // debug purpose
public:
    std::vector<BWAPI::TilePosition> btpath;
    std::set<BWAPI::Unit*> enemies;
	
	UnitsGroup();
	~UnitsGroup();

    BWAPI::Position center;
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
	
	virtual void updateCenter();
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

