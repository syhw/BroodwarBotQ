#pragma once
#define _CRTDBG_MAP_ALLOC       // anti-memory leaks
#include <stdlib.h>             // anti-memory leaks
#include <crtdbg.h>             // anti-memory leaks
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "Defines.h"
#include "Arbitrator.h"
#include "WorkerManager.h"
#include "SupplyManager.h"
#include "BuildManager.h"
#include "BuildOrderManager.h"
#include "TechManager.h"
#include "UpgradeManager.h"
#include "BaseManager.h"
#include "ScoutManager.h"
#include "MacroManager.h"
#include "MapManager.h"
#include "ProtossStrat.h"
#include "TerranStrat.h"
#include "ZergStrat.h"
#include "TimeManager.h"
#include "Regions.h"
#include "WarManager.h"
#include "ObjectManager.h"
#include "EUnitsFilter.h"
#include "EEcoEstimator.h"
#include "ETechEstimator.h"
#include "GoalManager.h"
#include "TimeManaged.h"
#include "InformationManager.h"
#include "BorderManager.h"
#include "UnitGroupManager.h"
#include "BuildingPlacer.h"
#include "ConstructionManager.h"
#include "MorphManager.h"
#include "ProductionManager.h"
#include "EnhancedUI.h"
#include "DefenseManager.h"

class BattleBroodAI;
extern BattleBroodAI* broodAI;

static bool analyzed;
static bool analysis_just_finished;
static BWTA::Region* home;
static BWTA::Region* enemy_base;
DWORD WINAPI AnalyzeThread();

class BattleBroodAI : public BWAPI::AIModule
{
public:
	bool show_visibility_data;
	BWTA::Region* home;
	BWTA::Region* enemy_base;
	bool analyzed;
	std::map<BWAPI::Unit*,BWAPI::UnitType> buildings;

	//Managers
	Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
	BaseManager* baseManager;
	BorderManager * borderManager;
	BuildingPlacer * buildingPlacer;
	BuildManager* buildManager;
	//Depends of BuildManager :
		ConstructionManager * constructionManager;
		MorphManager * morphManager;
		ProductionManager * productionManager;

	BuildOrderManager* buildOrderManager;
	SupplyManager* supplyManager;
	TechManager* techManager;
	UpgradeManager* upgradeManager;
	UnitGroupManager * unitGroupManager;
	InformationManager * informationManager;
	ScoutManager* scoutManager;
	MapManager* mapManager;
	WorkerManager* workerManager;
	Regions* regions;
	MacroManager* macroManager;
	WarManager* warManager;
	EUnitsFilter* eUnitsFilter;
	EEcoEstimator* eEcoEstimator;
	ETechEstimator* eTechEstimator;
	TimeManager* timeManager;
	GoalManager* goalManager;
	EnhancedUI * enhancedUI;
	DefenseManager * defenseManager;
    ObjectManager* objManager;
	bool showManagerAssignments;


    BattleBroodAI();
    ~BattleBroodAI();
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onPlayerLeft(BWAPI::Player* player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitCreate(BWAPI::Unit* unit);
	virtual void onUnitDestroy(BWAPI::Unit* unit);
	virtual void onUnitMorph(BWAPI::Unit* unit);
	virtual void onUnitShow(BWAPI::Unit* unit);
	virtual void onUnitHide(BWAPI::Unit* unit);
	virtual void onUnitRenegade(BWAPI::Unit* unit);
	virtual void onUnitDiscover(BWAPI::Unit* unit);
	virtual void onUnitEvade(BWAPI::Unit* unit);
	void drawStats(); //not part of BWAPI::AIModule
	void showPlayers();
	void showForces();
	void display();
};