#pragma once
#include <MacroManager.h>
#include <CSingleton.h>


class ProtossStrat: public MacroManager, public CSingleton<ProtossStrat>
{
	friend class CSingleton<ProtossStrat>;

	ProtossStrat();
	~ProtossStrat();

public:

	virtual void buildGeyser();
	virtual void createProdBuildings();
	virtual void createTechBuildings();
	virtual void buildDefenses();
	virtual void initWantedUnits();

	void buildGates();
};
