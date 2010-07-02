#include <BWTA.h>
#include "Goal.h"
#include "Regions.h"

class ScoutGoal: public Goal
{

private:
	Regions * regions;
public:
	ScoutGoal();
	virtual ~ScoutGoal();

//	virtual void achieve(UnitsGroup* ug);
	virtual void checkAchievement(UnitsGroup* ug);
};
