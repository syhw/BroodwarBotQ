#include <BWTA.h>
#include <ScoutManager.h>


using namespace BWAPI;
ScoutManager::ScoutManager( )
{
	regions = & Regions::Instance();
	goalManager = & GoalManager::Instance();
}

ScoutManager::~ScoutManager( )
{
}

void ScoutManager::update()
{
	if(this->newGoal()){
		pGoal p = getGoal();
		goalManager->newGoal(p);
	}
}

std::string ScoutManager::getName() const
{
	return "Scout Manager";
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
	//TODO
}



////////////////////////////NEW SECTION

void ScoutManager::onUnitCreate(BWAPI::Unit* unit){
	if(BWAPI::Broodwar->self()->supplyUsed() == 18 && unit->getType().isWorker() && !regions->EnemyFound()){
		BWAPI::Broodwar->printf("gotta find the enemy, ScoutManager created the objective");
		findEnemy();
	}
}


void ScoutManager::findEnemy(){
	//Create a new scoutGoal 
	pGoal g = pGoal(new ScoutGoal(SP_FINDENEMY));
	pSubgoal sb;

	//Scout the different possible bases
	myStartLocation = BWTA::getStartLocation(BWAPI::Broodwar->self());
	std::list<BWTA::BaseLocation*> path = getBestPath( BWTA::getStartLocations(), myStartLocation);
	Broodwar->printf("Number of bases : %d",path.size());
		
	for(std::list<BWTA::BaseLocation*>::iterator p=path.begin();p!=path.end();p++){
		sb=static_cast<pSubgoal>(new Subgoal(ST_VIEW,SC_ONCE,(*p)->getPosition()));
		g->addSubgoal(sb);
	}
	scoutGoals.push_back(g);
}

std::list<BWTA::BaseLocation*> ScoutManager::getBestPath( std::set<BWTA::BaseLocation* > baseLocations, BWTA::BaseLocation* myStartLocation) const
{
	std::list<BWTA::BaseLocation*> res;
	baseLocations.erase( myStartLocation);
	BWTA::BaseLocation* baseFrom = myStartLocation;
	while( !baseLocations.empty())
	{
		BWTA::BaseLocation* nearestBase = NULL;
		double dist = 0xFFFF;
		for( std::set<BWTA::BaseLocation* >::iterator b = baseLocations.begin(); b != baseLocations.end(); b++)
		{
			double dist2 = baseFrom->getGroundDistance( *b);
			if( dist2 <= 0) // Unreachable by walk.
				baseLocations.erase( nearestBase);
			if( dist2 < dist)
			{
				nearestBase = *b;
				dist = dist2;
			}
		}
		baseLocations.erase( nearestBase);
		res.push_back( nearestBase);
		baseFrom = nearestBase;
	}
	return res;
}

void ScoutManager::exploreRegion(BWTA::Region* region){

	BWTA::Polygon polygon = region->getPolygon();
	std::list<Position> to_see;
	bool insert=true;
	for(std::vector<Position>::iterator it = polygon.begin(); it != polygon.end(); ++it){
		for(std::list<Position>::iterator it_to_see = to_see.begin(); it_to_see != to_see.end(); ++it_to_see){
			if((*it_to_see).getDistance( (*it) ) <= BWAPI::UnitTypes::Protoss_Probe.sightRange())
				insert=false;
		}
		if (insert) {
			to_see.push_back(*it);
		}
		insert=true;
	}

	if(to_see.size() > 0){
		//Create the goal !
		pGoal goal = pGoal(new ScoutGoal(SP_EXPLOREREGION));
		double curDist;
		double maxDist=0;
		BWAPI::Position precPos=to_see.front();
		BWAPI::Position chosePos;
		goal->addSubgoal(pSubgoal(new Subgoal(ST_VIEW,SC_ONCE,to_see.front())));
		to_see.pop_front();
		int size = to_see.size();
		while(size > 0){
			
			for(std::list<Position>::iterator it = to_see.begin(); it != to_see.end(); ++it){
				curDist = it->getDistance(precPos);
				if(curDist > maxDist){
					maxDist=curDist;
					chosePos = (*it);
				}
			}
		precPos = chosePos;
		goal->addSubgoal(pSubgoal(new Subgoal(ST_VIEW,SC_ONCE,chosePos)));
		to_see.remove(chosePos);
		size --;
		maxDist=0;
		}
		
		this->scoutGoals.push_back(goal);
		BWAPI::Broodwar->printf("Goal to explore the base created");
	}
	

}
				//TOCHECK : I use a protoss_probe vision to explore the region
	

/*	
	std::list<Position>::iterator pos;
	BWTA::Polygon polygon = (*region).getPolygon();
	std::list<Position> to_see;
	for(std::vector<Position>::iterator it = polygon.begin(); it != polygon.end(); ++it)
		to_see.push_back((*it));
	
	//Select a first position added to the objectives
	//TODO select a not yet seen position
	Position current_pos= polygon.getNearestPoint(eStartLocation->getPosition());
	objectives.push_back(current_pos);
	to_see.remove(current_pos);

	double max_dist=0;
	double test_dist=0;
	Position current_target;
	int size=to_see.size();
	while(size >0){
		max_dist=0;
		current_target=current_pos;
		//Find the farthest point of current_pos
		for(pos = to_see.begin(); pos != to_see.end(); ++pos){
			test_dist=(*pos).getApproxDistance(current_pos);
			if(test_dist > max_dist){
				max_dist=test_dist;
				current_target = (*pos);
			}
		}
		Broodwar->printf("ou ? x %d  y %d", current_target.x(), current_target.y());
		add(current_target);
		Broodwar->drawCircleMap(current_target.x(),current_target.y(),200,Colors::Blue);
		Broodwar->drawCircle(CoordinateType::Screen,current_target.x(), current_target.y(),200,Colors::Yellow, true);
	//	void drawCircleMap(int x, int y, int radius, Color color = Color(BWAPI::Colors::Green), bool isSolid = false); 
		objectives.push_back(current_target);
		current_pos=current_target;
		to_see.remove(current_pos);
		size--;
	}
	Broodwar->printf("Nombre d'objectifs : %d", objectives.size());
	*/


int ScoutManager::newGoal() const {
	return scoutGoals.size();
}


pGoal ScoutManager::getGoal(){
	pGoal p=scoutGoals.front();
	scoutGoals.pop_front();
	return p;
}

void ScoutManager::onUnitShow(BWAPI::Unit* unit){
	if(unit->getPlayer()->isEnemy(BWAPI::Broodwar->self())){
		//enemy found, must explore his base
		this->exploreRegion(BWTA::getRegion(unit->getTilePosition()));
	}
}