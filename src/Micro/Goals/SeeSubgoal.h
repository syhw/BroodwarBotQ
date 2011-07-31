#pragma once
#include <BWAPI.h>
#include "Subgoal.h"

class SeeSubgoal : public Subgoal
{

public:
	SeeSubgoal(SubgoalLogic l, BWAPI::Position pos);
    SeeSubgoal(const SeeSubgoal& ssg);
	bool isRealized();
	void tryToRealize();
	double distanceToRealize();
	virtual double distanceToRealize(BWAPI::Position);
protected:
	BWAPI::Position _pos;
	bool _achieved;
};