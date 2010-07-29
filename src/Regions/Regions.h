#pragma once
#include "CSingleton.h"
#include <BWTA.h>
#include <BWAPI.h>
#include "BaseManager.h"
#include "TimeManager.h"
#include "MapManager.h"
#include <map>
#include <set>
#include <list>
#include <vector>
#include "BaseObject.h"


class RegionsUnitData
{
public:
	Unit* unit;
	UnitType unitType;
	Position position;
	int lastSeen;
	RegionsUnitData(Unit* unit);
	bool operator == (const RegionsUnitData& ud) const;
};

class RegionData
{
public:
	// TODO perhaps change vectors by sets (we need a quick find and don't care about the order)
	std::map<Player*, std::vector<RegionsUnitData> > buildings;	// list of enemy building seen in this region for each player.
	std::map<Player*, std::vector<RegionsUnitData> > units;      // list of enemy units seen in this region for each player.
	int lastSeen; // Last seen frame.
	int visited; //Number of the frame when a scout goal to explore was accomplished.
	// 0 means never
	RegionData();
	bool isOccupied() const;
	bool contain(Unit* unit) const;
	inline void add(Unit* unit);
};


class Regions : public CSingleton<Regions>, public BaseObject
{
	friend class CSingleton<Regions>;

private:
	Regions();
	~Regions();
	MapManager* mapManager;
	TimeManager* timeManager;
	inline BWTA::Region* findRegion(BWAPI::Position p);

public:
	void setDependencies(TimeManager * tm, MapManager * map);
	inline void addUnit(BWAPI::Unit* unit); // Add to the corresponding map (building/unit) in regionData. Refresh it if already present.
	void removeUnits();
	void addUnits();
	inline void removeUnit( Unit* unit);
	virtual void update();
	virtual std::string getName() const;
	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);
	void onUnitShow(BWAPI::Unit* unit);
	void onUnitHide(BWAPI::Unit* unit);
	void display() const;
	bool enemyFound() const;
	BWTA::Region* whereIsEnemy();

#ifdef BW_QT_DEBUG
	// Qt interface
	virtual QWidget* createWidget(QWidget* parent) const;
	virtual void refreshWidget(QWidget* widget) const;
#endif

	std::map<BWTA::Region*, RegionData> regionsData;

};
