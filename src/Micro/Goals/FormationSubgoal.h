#pragma once

#include "Subgoal.h"
#include "Formations.h"
#include <BWAPI.h>
#include "UnitsGroup.h"


class FormationSubgoal : public Subgoal
{
    pFormation formation;
    UnitsGroup* unitsgroup;
public:
    FormationSubgoal(SubgoalLogic l, pFormation f, UnitsGroup* ug);
	virtual bool isRealized();
	virtual void tryToRealize(UnitsGroup* ug);
	virtual double distanceToRealize(UnitsGroup* ug);
protected:
	virtual bool check();
};
