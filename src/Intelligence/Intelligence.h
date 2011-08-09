#pragma once
#include <PrecompiledHeader.h>
#include "Defines.h"
#include "Intelligence/EUnitsFilter.h"
#ifdef __ETECH_ESTIMATOR__
#include "Intelligence/ETechEstimator.h"
#endif
#include "Macro/InformationManager.h"
#include "Regions/MapManager.h"
#include "Intelligence/FirstScoutController.h"

class Intelligence : public CSingleton<Intelligence>
{
	friend class CSingleton<Intelligence>;
	Intelligence();
	~Intelligence();
	bool _launchedFirstScoutGoal;
	bool _launchedFirstExploreGoal;
	std::list<BWTA::BaseLocation*> _enemyBasesOrder; // to be used
public:
	std::set<BWTA::Region*> currentlyExploring;
	EUnitsFilter* eUnitsFilter;
#ifdef __ETECH_ESTIMATOR__
	ETechEstimator* eTechEstimator;
#endif
	int closestOnPath; // HACK TODO remove
	MapManager* mapManager;
	bool enemyRush;
	BWAPI::Race enemyRace;
	BWTA::BaseLocation* enemyHome;
	std::map<double, BWTA::BaseLocation*> enemyBasesOrder; // to be shared
	void update();
    void onUnitCreate(BWAPI::Unit* u);
    void onUnitDestroy(BWAPI::Unit* u);
    void onUnitShow(BWAPI::Unit* u);
    void onUnitHide(BWAPI::Unit* u);
	void onUnitMorph(BWAPI::Unit* u);
	void onUnitRenegade(BWAPI::Unit* u);
};