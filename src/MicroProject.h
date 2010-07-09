#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "Defines.h"
#include "Formations.h"
#include "MapManager.h"
#include "ObjectManager.h"
#include "Regions.h"
#include "UnitsGroup.h"

static bool analyzed;
static bool analysis_just_finished;
static BWTA::Region* home;
static BWTA::Region* enemy_base;
DWORD WINAPI AnalyzeThread();

class MicroAIModule : public BWAPI::AIModule
{
public:	    
    UnitsGroup* mm;
    MapManager* mapManager;
		ObjectManager* objectManager;
    Regions* regions;
	virtual void onStart();
#ifdef BW_QT_DEBUG
	MicroAIModule(QApplication** qapplication);
    QApplication** qapp;
#else
    MicroAIModule();
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