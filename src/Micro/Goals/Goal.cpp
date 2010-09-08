#include <algorithm>
#include "Goal.h"
#include "UnitsGroup.h"
#include "float.h"
#include "Defines.h"

Goal::~Goal()
{
}
Goal::Goal()
: status(GS_NOT_ATTRIBUTED)
, firstFrame(BWAPI::Broodwar->getFrameCount())
{
}

Goal::Goal(UnitsGroup * ug)
: status(GS_IN_PROGRESS)
, unitsGroup(ug)
, firstFrame(BWAPI::Broodwar->getFrameCount())
{
}

Goal::Goal(UnitsGroup * ug, pSubgoal s)
: status(GS_IN_PROGRESS)
, unitsGroup(ug)
, firstFrame(BWAPI::Broodwar->getFrameCount())
{
    addSubgoal(s);
}

void Goal::achieve()
{
	if (this->status != GS_NOT_ATTRIBUTED)
    {
		checkAchievement();
			
		if (this->status != GS_ACHIEVED)
        {
			double test;
			pSubgoal selected;
			double min = DBL_MAX;			
			for (std::list<pSubgoal>::iterator it = subgoals.begin(); it != subgoals.end(); ++it)
            {
				if (!(*it)->isRealized())
                {
					test = (*it)->distanceToRealize();
					if (test >= 0 && test < min)
                    {
						min = test;
						selected = (*it);
					}
				}
			}
			if (min > 0 && min < DBL_MAX)
            {
				selected->tryToRealize();
			} else {
				//TODO
                //BWAPI::Broodwar->printf("(Goal) Tell Louis this situation happened subgoals size %d", subgoals.size());
			}
		}
	}

}

void Goal::checkAchievement()
{

    if (this->status!=GS_ACHIEVED && this->status!=GS_NOT_ATTRIBUTED)
    {
        //All the subgoals are tested because the check function might validate
        //some subgoals
        bool res_and=true;
        bool res_or=false;

        for (std::list<pSubgoal>::iterator p = subgoals.begin(); p != subgoals.end(); )
        {
            if ((*p)->getLogic() == SL_AND)
            {
                //AND case 
                if (!(*p)->isRealized())
                {
                    res_and=false;
                    ++p;
                } else
                    subgoals.erase(p++);
            } else {
                //OR case
                if ((*p)->isRealized())
                    res_or = true;
                ++p;
            }
        }

        if(res_and || res_or)
        {
            this->status= GS_ACHIEVED;
#ifdef __DEBUG_GABRIEL__
            BWAPI::Broodwar->printf("\x13 \x04 goal finished in ~ %d seconds", (BWAPI::Broodwar->getFrameCount() - firstFrame)/24);
#endif
        }
    }
}

void Goal::addSubgoal(pSubgoal s)
{
	if (this->status != GS_NOT_ATTRIBUTED)
    {
		s->setUnitsGroup(unitsGroup);
	}
	this->subgoals.push_back(s);
}

GoalStatus Goal::getStatus() const
{
	return status;
}

void Goal::setStatus(GoalStatus s)
{
	status = s;
}



void Goal::setUnitsGroup(UnitsGroup * ug)
{
	this->status = GS_IN_PROGRESS;
	for (std::list<pSubgoal>::iterator it = this->subgoals.begin(); it != subgoals.end(); ++it)
    {
		(*it)->setUnitsGroup(ug);
	}
	this->unitsGroup = ug;
}


int Goal::estimateDistance(BWAPI::Position p)
{
	
	for (std::list<pSubgoal>::iterator it = this->subgoals.begin(); it != this->subgoals.end(); ++it)
    {
		if ((*it)->distanceToRealize(p) > 0)
        {
			return (*it)->distanceToRealize(p);
		}
	}
	return -1;
}
