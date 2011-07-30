#pragma once
#include <boost/shared_ptr.hpp>
#include <BWAPI.h>
#include "Micro/UnitsGroup.h"

typedef enum
{
	SL_AND    = 0,
	SL_OR     = 1
} SubgoalLogic;
	
class Subgoal
{
public:
    Subgoal(SubgoalLogic l, UnitsGroup* ug);
    Subgoal(const Subgoal& sg);
    ~Subgoal();
    //Accessors
	SubgoalLogic getLogic() const;
	void setUnitsGroup(UnitsGroup * ug);
	//Check accomplishment
	virtual bool isRealized() = 0; //Return if the subgoal is accomplished
	virtual void tryToRealize() = 0; //Give suggestions to the UnitsGroup
	virtual double distanceToRealize() = 0; //Return an estimated distance to accomplish the Subgoal
	//if the return value is negative, it means that the subgoal cannot give suggestions to the UnitsGroup
protected:
	SubgoalLogic _logic;
	UnitsGroup* _unitsGroup;
};