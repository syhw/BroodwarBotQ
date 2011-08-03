#pragma once
#include "Micro/UnitsGroup.h"

typedef enum
{
	SL_AND    = 0,
	SL_OR     = 1
} SubgoalLogic;
	
/// Note to self:
/// This class should _not_ have _unitsGroup (think shortcut Kill subgoals)
/// but I would like all Subgoals to be able to _do something_ so that they
/// don't get stuck unrealized. So shortcuts send distanceToRealize() = -1
/// but _can_ tryToRealize()
class Subgoal
{
public:
    Subgoal(SubgoalLogic l, UnitsGroup* ug);
    Subgoal(SubgoalLogic l);
    Subgoal(const Subgoal& sg);
    virtual ~Subgoal();
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