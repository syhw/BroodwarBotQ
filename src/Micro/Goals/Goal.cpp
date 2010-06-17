#include <algorithm>
#include "Goal.h"
#include "UnitsGroup.h"
/*
Goal::Goal()
: type(GT_ND)
, status(GS_IN_PROGRESS)
, formation(NULL)
, purpose("Default Goal") 
, achiviedCpt(0)
{
}
*/
Goal::Goal(const Goal& g)
: type(g.type)
, status(g.status)
, formation(g.formation)
, purpose(g.purpose) 
, achiviedCpt(0)
{
}

Goal::Goal(const char* text, pFormation f)
: type(GT_ND)
, status(GS_IN_PROGRESS)
, formation(f)
, purpose(text) 
, achiviedCpt(0)
{
}

Goal::~Goal()
{
}

void Goal::achieve(UnitsGroup* ug)
{
	status = GS_IN_PROGRESS;
	achiviedCpt = 0;
	ug->formation(this->formation);
}

void Goal::checkAchievement(UnitsGroup* ug)
{
	bool allIdle = true;
	for(unsigned int i = 0; i < ug->getNbUnits(); i++)
	{
		if (!(*ug)[i].unit->isIdle())
			allIdle = false;
	}
	if( !allIdle)
	{
		achiviedCpt = 0;
		status = GS_IN_PROGRESS;
	}
	if (allIdle) achiviedCpt++;
	if (achiviedCpt > 2) status = GS_ACHIEVED;
}
