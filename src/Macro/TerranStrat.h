#pragma once
#include <MacroManager.h>
#include <CSingleton.h>


class TerranStrat: public MacroManager, public CSingleton<TerranStrat>
{
	friend class CSingleton<TerranStrat>;

	TerranStrat();
	~TerranStrat();
public:

	virtual void buildGeyser();
	virtual void createTechBuildings();
	virtual void createProdBuildings();
	virtual void initWantedUnits();
};
