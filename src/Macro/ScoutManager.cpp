#include <BWTA.h>
#include <ScoutManager.h>

ScoutManager::ScoutManager( )
{
	scoutObjectives = & ScoutObjectives::Instance();
	arbitrator = & Arbitrator::Arbitrator<BWAPI::Unit*,double>::Instance();
	regions = & Regions::Instance();
	desiredScoutCount = 0;
	this->positionsToScout = scoutObjectives->get_Objectives();
	showPath();
}

ScoutManager::~ScoutManager( )
{
}

void ScoutManager::onOffer(std::set<BWAPI::Unit*> units)
{
	for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
	{
		if ((*u)->getType().isWorker() && needMoreScouts())
		{
			arbitrator->accept(this, *u);
			addScout(*u);
		}
		else
			arbitrator->decline(this, *u, 0);
	}
}

void ScoutManager::onRevoke(BWAPI::Unit *unit, double bid)
{
	scouts.erase(unit);
}

void ScoutManager::update()
{
	//if (!(Broodwar->getFrameCount() % (24*50)))
		//checkEmptyXP();

	//TODO find a way to remove these test in mid game
	if(!desiredScoutCount)
		setScoutCount(1);
	
	if(scoutObjectives->got_objectives() && this->positionsToScout.size()==0){
		this->positionsToScout = this->scoutObjectives->get_Objectives();
		Broodwar->printf("Objectives acquired");
	}

	if (needMoreScouts())
	{
		requestScout(/* bid = */ 12); // Bid 12.
	}
	else
	{
		while (scouts.size()>desiredScoutCount)
		{
			arbitrator->setBid(this, scouts.begin()->first,0);
			scouts.erase(scouts.begin());
		}
	}
	updateScoutAssignments();

	//TODO Once all enemies found, stop scouting & Remove positions scouted by allies
//	const map<Region*, RegionData>& regionsData = regions->regionsData;
//	for( map<Region*, RegionData>::const_iterator r = regionsData.begin(); r != regionsData.end(); r++)
//	{
//		const RegionData& rd = ;
//		for( map<Player*, list<Unit*> >::const_iterator rd = r->second.buildings.begin(); rd != r->second.buildings.end(); rd++)
//		{
//			const Player* player = rd->first;
//			const list<Unit*>* buildings = rd->second;
			// Once all enemies found, stop scouting
//			const std::set<BaseLocation*>& basesLocPerRegion = r->first->getBaseLocations();
//			for( std::set<BaseLocation*>::const_iterator blpr = basesLocPerRegion.begin(); blpr != basesLocPerRegion.end(); blpr++)
//			{
//				Position p = (*blpr)->getPosition();

//			}

			// Remove positions scouted by allies
			
//		}
//	}


}

std::string ScoutManager::getName() const
{
	return "Scout Manager";
}

void ScoutManager::onUnitDestroy(BWAPI::Unit* unit)
{
	scouts.erase(unit);
}

void ScoutManager::setScoutCount(int count)
{
	this->desiredScoutCount=count;
}

bool ScoutManager::isScouting() const
{
	return scouts.size() >= 1;
}

bool ScoutManager::needMoreScouts() const
{
	return scouts.size() < desiredScoutCount &&
		positionsToScout.size();
}

void ScoutManager::requestScout(double bid)
{
	// Bid on all completed workers.
	std::set<BWAPI::Unit*> myPlayerUnits=BWAPI::Broodwar->self()->getUnits();
	for(std::set<BWAPI::Unit*>::iterator u = myPlayerUnits.begin(); u != myPlayerUnits.end(); u++)
	{
		if ((*u)->isCompleted() && (*u)->getType().isWorker())
		{
			arbitrator->setBid(this, *u, bid);
		}
	}
}

void ScoutManager::addScout(BWAPI::Unit* u)
{
	ScoutData temp;
	scouts.insert(std::make_pair(u,temp));
}

void ScoutManager::updateScoutAssignments()
{
	// Remove scout positions once seen and set the scouter to idle.
	std::map<BWAPI::Unit*, ScoutData>::iterator u;
	for(u = scouts.begin(); u != scouts.end(); u++)
	{
		if ( (*u).second.mode == ScoutData::Searching)
		{
			double dist = (*u).first->getPosition().getDistance((*u).second.target);
			if( (0 < dist) && (dist < (*u).first->getType().sightRange()))
			{
				for(std::list<BWAPI::Position>::iterator p=positionsToScout.begin();p!=positionsToScout.end();p++)
				{
					if (*p==(*u).second.target)
					{
						positionsToScout.erase(p);
						break;
					}
				}
				(*u).second.mode = ScoutData::Idle;
			}
		}
	}

	// Set scouts to scout.
	if (positionsToScout.size() > 0) // are there still positions to scout?
	{
		std::list<BWAPI::Position>::iterator p;
		for( u = scouts.begin(), p = positionsToScout.begin();
			u != scouts.end() && p != positionsToScout.end();
			u++) 
		{ // for
			if ((*u).second.mode == ScoutData::Idle)
			{
				(*u).second.mode = ScoutData::Searching;
				(*u).first->rightClick(*p);
				(*u).second.target = *p;
				p++;
			}
		} // for
	}
	else
	{
		// There is nothing to do, free the idle unit
		for( u = scouts.begin(); u != scouts.end(); u++) 
			if ((*u).second.mode == ScoutData::Idle)
				arbitrator->removeBid( this, u->first);
	}

}



void ScoutManager::scoutAllEnemies()
{
	// TODO
}

void ScoutManager::counterWall()
{
	// TODO
	// se placer ou construire a l'endroit du wall
	// pour photon ensuite ?
}

void ScoutManager::counterBuild()
{
	// TODO
	// En placant le drone aux endroits de construction.
	// Si terran, attaquer le scv qui construit
}

void ScoutManager::harassWorkers()
{
	// TODO
	// hit and run dans les drones aux min
}

void ScoutManager::checkEmptyXP()
{
	// Be sure we have at least a scout
	if(!desiredScoutCount)
		setScoutCount(1);

	//Broodwar->printf( "########  checkEmptyXP  !!");
	// Get the base set still not occupied.
	std::set<BWTA::BaseLocation* > baseLocToVisit;
	const std::set<BWTA::BaseLocation*>& allBaseLoc = BWTA::getBaseLocations();
	for (std::set<BWTA::BaseLocation*>::const_iterator itBL = allBaseLoc.begin(); itBL != allBaseLoc.end(); ++itBL)
	{
		BWTA::Region* baseRegion = (*itBL)->getRegion();
		if( ! regions->regionsData[baseRegion].isOccupied() &&
			BWTA::getGroundDistance(baseRegion->getCenter(), Broodwar->self()->getStartLocation()) > 0)
			baseLocToVisit.insert( *itBL);
	}

	// Add it to the position to scout
	BWTA::BaseLocation* myStartLocation = BWTA::getStartLocation(BWAPI::Broodwar->self());
	std::list<BWTA::BaseLocation*> orderedBaseLoc = scoutObjectives->getBestPath(baseLocToVisit, myStartLocation);
	for(std::list<BWTA::BaseLocation*>::iterator it = orderedBaseLoc.begin(); it != orderedBaseLoc.end(); ++it)
		positionsToScout.push_back((*it)->getPosition());
}


void ScoutManager::showPath()
{
	BWAPI::Broodwar->sendText( "I will scout here.");
	for( std::list<BWAPI::Position>::iterator p = positionsToScout.begin(); p != positionsToScout.end(); p++)
	{
		// display all the position to scout on the minimap for the others.
		Broodwar->pingMinimap( p->x(), p->y());
	}
}
