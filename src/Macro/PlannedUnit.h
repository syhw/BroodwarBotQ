#pragma once
#include <BWAPI.h>

typedef enum {
	TYPE_RATIO, TYPE_NUMBER
} PlannedType;

class PlannedUnit
{
public:
	PlannedUnit(){ completedNb = 0;criteria = 0;};
	~PlannedUnit(){};

	PlannedType plannedType;	// RATIO / FIXED_NUMBER
	int plannedValue;					// ratio/number value
	int completedNb;					// Nb units of this type in game . Maybe ever implemented by BWAPI::Broodwar->self()->completedUnitCount(type), have to check.
	double criteria;					// actual ratio for this type = completed / plannedValue.
};
