#pragma once
#include <MacroManager.h>
#include <CSingleton.h>


class ProtossStrat: public MacroManager, public CSingleton<ProtossStrat>
{
		friend class CSingleton<ProtossStrat>;
private:
	typedef enum
{
	Arbiter,
	Archon,
	Carrier,
	Corsair,
	Dark_Archon,
	Dark_Templar,
	Dragoon,
	High_Templar,
	Observer,
	Reaver,
	Scout,
	Shuttle,
	Zealot
} UnitTypeEnum;


	ProtossStrat();
	~ProtossStrat();

public:
	void onStart();
	void update();
	virtual void buildGeyser();
	virtual void createProdBuildings();
	virtual void createTechBuildings();
	virtual void buildDefenses();
	void setScoutTime();
	void buildGates();
	void eRush(); // To call on detection of a rush
	void produceUnits();
	int priority[13];
	int needed[13];
	void buildUnits();

};
