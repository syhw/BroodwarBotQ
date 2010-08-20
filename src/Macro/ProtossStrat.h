#pragma once
#include <MacroManager.h>
#include <CSingleton.h>


class ProtossStrat: public MacroManager, public CSingleton<ProtossStrat>
{
	friend class CSingleton<ProtossStrat>;

	ProtossStrat();
	~ProtossStrat();

public:
	void onStart();
	void update();
	virtual void buildGeyser();
	virtual void createProdBuildings();
	virtual void createTechBuildings();
	virtual void buildDefenses();
	virtual void initWantedUnits();
	void setScoutTime();
	void buildGates();
	void eRush(); // To call on detection of a rush
};
