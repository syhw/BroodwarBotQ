#pragma once
#include "CSingleton.h"
#include "TimeManager.h"
#include <BWAPI.h>
#include <BWTA.h>


class EEcoEstimator : public CSingleton<EEcoEstimator>
{
	friend class CSingleton<EEcoEstimator>;
public:
	void setDependencies();
	virtual void update();
	int m_per_mn() const;//Requires a first call to update workers else uses theorical number of workers
	int g_per_mn() const;//Requires a first call to update workers else uses theorical number of workers
	void update_workers(int mworkers, int gworkers);
	void add_workers(int mworkers, int gworkers);
	void rm_workers(int mworkers, int gworkers);

	void update_minerals_geyzers(int patch, int geyzers);

	int get_theo_workers() const;
	void start_gaz(int gworkers);
	//gworkers correspond to the number of workers on the refinery
	//Take care, gworkers is added directly to the number of workers on gaz
	//And the number of gaz_exploited is increased

private:
	EEcoEstimator();
	~EEcoEstimator();
	TimeManager* timeManager;
	int mWorkers;// workers on mineral
	int gWorkers;// workers on gaz
	int totWorkers;
	int patch_owned;
	int gaz_exploited;
	int gaz_owned;
	int seconds;
	int theo_workers;
};