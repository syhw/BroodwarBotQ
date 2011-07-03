#include <PrecompiledHeader.h>
#include "Micro/Goals/FindEnemyGoal.h"

FindEnemyGoal::FindEnemyGoal()
{
	scoutController = & ScoutController::Instance();
	//Scout the different possible bases
	for (std::set<BWTA::BaseLocation*>::const_iterator p = BWTA::getStartLocations().begin();
		p != BWTA::getStartLocations().end(); ++p)
    {
		if ((*p) != BWTA::getStartLocation(BWAPI::Broodwar->self()))
			addSubgoal(pSubgoal(new SeeSubgoal(SL_AND, (*p)->getPosition())));
	}
	addSubgoal(pSubgoal(new FindSubgoal(SL_OR)));
}

void FindEnemyGoal::achieve()
{
	if (this->status != GS_NOT_ATTRIBUTED)
    {
		checkAchievement();
			
		if (this->status != GS_ACHIEVED)
        {
			double test;
			pSubgoal selected;
			double min = 10000000000.0;
			
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
				Broodwar->printf("ERROR IN FindEnemyGoal");
				// TODO
                // Problem if here
			}
		}
        else
        {
            for (std::list<pSubgoal>::iterator it = subgoals.begin();
                it != subgoals.end(); ++it)
            {
                if ((*it)->getLogic() == SL_OR && (*it)->isRealized())
                {

                }
            }
        }
	}
}