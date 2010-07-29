#pragma once
#include "BWAPI.h"
#include "CSingleton.h"
#include <stdlib.h>


class Building {
	public:
		Building(BWAPI::UnitType type, double probToHave); //Automatically set produces, upgrades and techs
		BWAPI::UnitType type;
		std::list<Building> needed;
		double probToHave;
};

class Tech {
public:
	Tech(BWAPI::TechType type, double probToHave);
	BWAPI::TechType type;
	double probToHave;
};

class Upgrade {
public:
	Upgrade(BWAPI::UpgradeType type, double probToHave);
	BWAPI::UpgradeType type;
	double probToHave;
};


class ETechEstimator : public CSingleton<ETechEstimator>
{
	friend class CSingleton<ETechEstimator>;
public:

	double canProduce(BWAPI::UnitType ut);
	double hasTech(BWAPI::TechType tt);
	double hasUpgrade(BWAPI::UpgradeType upt);
	void updateProbs();
	void sayProbs();
private:
		
	ETechEstimator();
	~ETechEstimator();
	std::list<Building> buildings;
	Building getBuilding(BWAPI::UnitType ut);

};