#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "Defines.h"
#include "Intelligence/Intelligence.h"
#include "Macro.h"
#include "Micro.h"

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

	Intelligence* intelligence;
	Macro* macro;
	Micro* micro;
	TimeManager* timeManager;

	bool showManagerAssignments;
	
    BattleBroodAI();
    ~BattleBroodAI();
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player* player, std::string text);
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
	virtual void onSaveGame(std::stringbuf gameName);

	//not part of BWAPI::AIModule
	void drawStats(); 
	void showPlayers();
	void showForces();
};