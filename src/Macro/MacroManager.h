#pragma once
#include <BaseManager.h>
#include <BuildManager.h>
#include <BuildOrderManager.h>
#include <ProductionManager.h>
#include <WorkerManager.h>
#include <BWAPI.h>
#include <BWTA.h>
#include <list>
#include <map>
#include <set>
#include <PlannedUnit.h>
#include <Util.h>
#include "BaseObject.h"


using namespace BWAPI;
using namespace BWTA;
using std::list;
using std::map;
using std::set;

#define MAX_WORKERS_NB 30
#define TRAIN_COOLDOWN 75

/*
class BuildingInfo
{
public:
BuildingInfo(){};
~BuildingInfo(){};


list<Unit*> Buildings;

}
*/


/*
TODO
A partir du nb/ratio d'units defini
creation d'units: creer selon le ratio en fct (remaining places, units existantes)
creation de bats: creer si full & eco montante
creation de tech: creer selon le ratio et units/bats nb requiered

priorite entre les type ? rejoins requiered pour la tech
*/

class MacroManager: public BaseObject
{
public:
    std::set<BWAPI::Unit*> nexuses; // provisory
    std::set<BWAPI::Unit*> gateways; // provisory
    std::set<BWAPI::Unit*> forges; // provisory

	MacroManager();
	~MacroManager();
	void setDependencies();
	virtual void onStart();
	std::string getName() const;
	virtual void eRush();
	int priority[13];
	int needed[13];

	void onUnitDestroy(BWAPI::Unit* unit);
	virtual void update();
	virtual void onUnitCreate(BWAPI::Unit* unit);
	virtual void buildGeyser() = 0;
	void trainWorkers();
	virtual void createProdBuildings() = 0;
	virtual void trainUnits();
	virtual void createTechBuildings();
	void researchTech();
	void expand();
	virtual void buildDefenses();
	virtual void initWantedUnits();
    virtual void someAir();

	int remainingTrainingPlace(UnitType type);
	bool canTrainUnits(UnitType unitType, int remainingPlaces);
	bool canCreateTechBuildings(UnitType techBuilding, UnitType buildingRequiered, int nbRequieredBuilding);
	bool canCreateDefenseBuildings(UnitType techBuilding, UnitType buildingRequiered);
	bool shouldExpand();

protected:
	BuildOrderManager* buildOrderManager;
	ProductionManager* productionManager;
	BuildManager* buildManager;
	BaseManager* baseManager;
	WorkerManager* workerManager;

	//TODO initialiser toutes ces structures pendant onstart() en cas de chargement

	// Buildings
	// Mixer ces trois en map<UnitType, list<Unit*>> buildings
	set<UnitType> techBuildings;
	map<UnitType, list<Unit*> > prodBuildings; // list of buildings for each type
	map<UnitType, list<Unit*> > defBuildings; // list of buildings for each type
	map<UnitType, int> underConstruction; // <Building Type, coolDown or true/false >
	// Units
	map<UnitType, PlannedUnit> wantedUnits;// wanted ratio for each unit.
	bool expanding;
	int firstScout;
	bool scouting;
    bool _shouldExpand;
};
