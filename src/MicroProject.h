#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "UnitsGroup.h"
#include "MapManager.h"
#include "Regions.h"
#include "Formations.h"
#include "ObjectManager.h"

#define BW_QT_DEBUG 1
#ifdef BW_QT_DEBUG
#include <QtGui/QApplication>
static QApplication* qapplication = NULL;
#endif
class MicroAIModule;
static MicroAIModule* broodAI = NULL;

static bool analyzed;
static bool analysis_just_finished;
static BWTA::Region* home;
static BWTA::Region* enemy_base;
DWORD WINAPI AnalyzeThread();

class MicroAIModule : public BWAPI::AIModule, public ObjectManager
{
public:	    
    UnitsGroup* mm;
    MapManager* mapManager;
    Regions* regions;
	virtual void onStart();
#ifdef BW_QT_DEBUG
	MicroAIModule(QApplication** qapplication);
    QApplication** qapp;
#else
    MicroAIModule()
#endif
    ~MicroAIModule();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	//virtual void onRemove(BWAPI::Unit* unit);
	virtual bool onSendText(std::string text);
    virtual void onUnitCreate(Unit* unit);
    virtual void onUnitDestroy(Unit* unit);
    virtual void onUnitShow(BWAPI::Unit* unit);
	virtual void onUnitHide(BWAPI::Unit* unit);
	void showStats(); //not part of BWAPI::AIModule
	void showPlayers();
	void showForces();
	std::map<BWAPI::Unit*,BWAPI::UnitType> buildings;
};