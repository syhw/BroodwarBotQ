#include <BWTA.h>
#include "Goal.h"
#include "Regions.h"

typedef enum
{
	SP_UNDEFINED   = 0,
	SP_FINDENEMY   = 1,
	SP_EXPLOREREGION = 2

} ScoutPurpose;


class ScoutGoal: public Goal
{
private:
	Regions * regions;
	ScoutPurpose purpose;
public:
	ScoutGoal(ScoutPurpose sp);
	virtual ~ScoutGoal();
	virtual void checkAchievement(UnitsGroup* ug);
};
