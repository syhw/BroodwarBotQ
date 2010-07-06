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

typedef struct __enemy
{
    BWAPI::Unit* self;
    int damageTaken;
} cEnemy;


class UnitsGroup
{
private:
	pGoal lastGoal; // the last goal is copied from the last executed goal in 'goals' and is keep to give order to new units added to a group (to avoid empty 'goals' for new units).
	//No longer used theorically


	int totalHP;
	int totalPower;
    BWAPI::Position center;
    std::vector<pBayesianUnit> units;
    std::map<BWAPI::Unit*, std::list<pBayesianUnit> > attackersEnemy;
    void goonMicro(pBayesianUnit u);
    void zealotMicro(pBayesianUnit u);
    void dragoonIA(std::set<BWAPI::Unit*> enemies, double maxRangeGoonEnemy);
	std::list<pGoal> goals; // list of goals to accomplish
	GoalManager* goalManager;
public:
	
	UnitsGroup();
	~UnitsGroup();

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
	virtual const pGoal getLastGoal() const;
	//virtual bool checkInFormation();
	//virtual bool checkAtDestination();
	virtual void updateCenter();
    virtual BWAPI::Position getCenter() const;

	// Micro tech (to be placed in other classes. For instance DistantUnits...)
	void keepDistance();

	// Units interface
    virtual void onUnitDestroy(BWAPI::Unit* u);
    virtual void onUnitShow(BWAPI::Unit* u);
    virtual void onUnitHide(BWAPI::Unit* u);
    virtual void takeControl(BWAPI::Unit* u);
    virtual void giveUpControl(BWAPI::Unit* u);
	bool emptyUnits();
	bool emptyGoals();
	unsigned int getNbUnits() const;
    int getTotalHP() const;
    std::vector<pBayesianUnit>* getUnits();
    std::map<BWAPI::Unit*, std::list<pBayesianUnit> >& getAttackersEnemy();

    static BWAPI::Unit* findWeakestEnemy(std::set<BWAPI::Unit*> enemies_in_range);

#ifdef _UNITS_DEBUG
    void selectedUnits(std::set<pBayesianUnit>& u);
#endif
	const BayesianUnit& operator[](int i);
	void accomplishGoal();
};

