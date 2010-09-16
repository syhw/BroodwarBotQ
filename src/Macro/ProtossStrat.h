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
    virtual void someAir();
	void setScoutTime();
	void buildGates();
	void eRush(); // To call on detection of a rush
	void buildUnits();

};
