#pragma once
#include <MacroManager.h>
#include <CSingleton.h>


class ZergStrat: public MacroManager, public CSingleton<ZergStrat>
{
	friend class CSingleton<ZergStrat>;

	ZergStrat();
	~ZergStrat();
public:

	virtual void buildGeyser();
	virtual void createProdBuildings();
	virtual void createTechBuildings();
	virtual void initWantedUnits();
	virtual void trainUnits();
};
