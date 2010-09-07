#pragma once

#include "Subgoal.h"
#include "Formations.h"
#include <BWAPI.h>
#include "UnitsGroup.h"


class FormationSubgoal : public Subgoal
{
    pFormation formation;
public:
    FormationSubgoal(SubgoalLogic l, pFormation f);
    FormationSubgoal(const FormationSubgoal& fsg);
	virtual bool isRealized();
	virtual void tryToRealize();
	virtual double distanceToRealize();
protected:
	virtual bool check();
};
