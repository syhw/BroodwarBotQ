#pragma once
#include <BWAPI.h>
#include "Subgoal.h"

class FindSubgoal : public Subgoal
{
public:
	FindSubgoal(SubgoalLogic l);
	virtual bool isRealized();
	virtual void tryToRealize();
	virtual double distanceToRealize();
protected:
	virtual bool check();
	bool achieved;
};