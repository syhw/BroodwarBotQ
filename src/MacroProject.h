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
#include "MicroManager.h"
#include "ObjectManager.h"
#include "EUnitsFilter.h"
#include "EEcoEstimator.h"
#include "ScoutObjectives.h"

#ifdef BW_QT_DEBUG
#include <QtGui/QApplication>
#include "MainWindow.h"
static QApplication* qapplication = NULL;
static MainWindow* qmainwindow = NULL;
#endif


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
	Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
	BaseManager* baseManager;
	BuildManager* buildManager;
	BuildOrderManager* buildOrderManager;
	MacroManager* macroManager;
	MicroManager* microManager;
	Regions* regions;
	ScoutManager* scoutManager;
	SupplyManager* supplyManager;
	TechManager* techManager;
	TimeManager* timeManager;
	UpgradeManager* upgradeManager;
	WorkerManager* workerManager;
    MapManager* mapManager;
    EUnitsFilter* eUnitsFilter;
	EEcoEstimator* eEcoEstimator;
	ScoutObjectives* scoutObjectives;
	bool showManagerAssignments;
    ObjectManager* objManager;

#ifdef BW_QT_DEBUG
	BattleBroodAI(QApplication** qappli, MainWindow** qmain);
    QApplication** qapp;
#else
    BattleBroodAI();
#endif
    ~BattleBroodAI();
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual bool onSendText(std::string text);
	virtual void onPlayerLeft(BWAPI::Player* player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitCreate(BWAPI::Unit* unit);
	virtual void onUnitDestroy(BWAPI::Unit* unit);
	virtual void onUnitMorph(BWAPI::Unit* unit);
	virtual void onUnitShow(BWAPI::Unit* unit);
	virtual void onUnitHide(BWAPI::Unit* unit);
	virtual void onUnitRenegade(BWAPI::Unit* unit);
	void drawStats(); //not part of BWAPI::AIModule
	void showPlayers();
	void showForces();
	void display();
};
