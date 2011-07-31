#pragma once

#include "Subgoal.h"
#include "Micro/Formations.h"
#include <BWAPI.h>
#include "Micro/UnitsGroup.h"


class FormationSubgoal : public Subgoal
{
    pFormation formation;
public:
    FormationSubgoal(SubgoalLogic l, UnitsGroup* ug, pFormation f);
    FormationSubgoal(const FormationSubgoal& fsg);
	virtual bool isRealized();
	virtual void tryToRealize();
	virtual double distanceToRealize();
protected:
	virtual bool check();
};
