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
    std::list<pBayesianUnit> unitsAvailables;
    std::map<int, cEnemy> enemiesInSight;
    std::set<BWAPI::Unit*> enemies;
	int totalHP;
	int totalPower;
    std::vector<pBayesianUnit> units;
    std::map<BWAPI::Unit*, std::list<pBayesianUnit> > attackersEnemy;
    void goonMicro(pBayesianUnit u);
    void zealotMicro(pBayesianUnit u);
    void dragoonIA(std::set<BWAPI::Unit*> enemies, double maxRangeGoonEnemy);
    
	std::list<pGoal> goals; // list of goals to accomplish
	GoalManager* goalManager;
    /// Mets à jour la liste des ennemis en vue de l'UnitsGroup, et ces derniers sont triés par ordre croissant de HP/SP
    void updateEnemiesInSight(std::vector<pBayesianUnit>::iterator it);
    /// Mets à jour la liste des unités capables de sélectionner une nouvelle cible
    void updateUnitsAvaibles(std::vector<pBayesianUnit>::iterator it);
    /// Assignation des cibles aux unités du UnitsGroup qui sont disponible pour sélectionner une nouvelle cible
    void updateTargetOfUnitsAvailables();
    /// Affiche les cibles des unités du UnitsGroup
    void displayTargets();
public:
	
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
	
	//virtual bool checkInFormation();
	//virtual bool checkAtDestination();
    int size();
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
	void switchMode(unit_mode um);
	void idle();
};

