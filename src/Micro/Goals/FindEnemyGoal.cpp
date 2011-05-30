#include <PrecompiledHeader.h>
#include "FindEnemyGoal.h"

FindEnemyGoal::FindEnemyGoal()
{
	scoutController = & ScoutController::Instance();
	warManager = & WarManager::Instance();

	//Scout the different possible bases
	BWTA::BaseLocation * myStartLocation = BWTA::getStartLocation(BWAPI::Broodwar->self());
	std::set<BWTA::BaseLocation*> path = BWTA::getStartLocations();
    if (Broodwar->getFrameCount() > 4320) // 4 minutes, in case of CC lift
        path = BWTA::getBaseLocations();
		
	for (std::set<BWTA::BaseLocation*>::iterator p=path.begin(); p!=path.end(); p++)
    {
		if ((*p) != myStartLocation)
        {
            Position debug = (*p)->getPosition();
			addSubgoal(pSubgoal(new SeeSubgoal(SL_AND, (*p)->getPosition())));
		}
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