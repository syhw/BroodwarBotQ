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
	pGoal g = pGoal(new ScoutGoal());
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

void ScoutManager::onUnitShow(BWAPI::Unit* unit){
//TO REMOVE FROM THIS FILE

//We assume that if the enemy was not spotted and we find a ResourceDepot, then it is the main base : requires fast scout

	/*
	if(unit->getType().isResourceDepot()){
	
		//Find the right base location
		std::set<BWTA::BaseLocation*> res = BWTA::getStartLocations();
		for(std::set<BWTA::BaseLocation* >::iterator b = res.begin(); b!= res.end(); ++b){
			if( (*b)->getTilePosition() == unit->getTilePosition() ){
				eStartLocation = (*b);
				break;
			}
		}
		
		if(eStartLocation == NULL)
			Broodwar -> printf("eStartLocation is NULL, problem...");

		Broodwar->printf("Enemy main base found, waiting for objectives");
		BWTA::Region* region = eStartLocation->getRegion();

		exploreRegion(region);
		
	}
	*/
}

void ScoutManager::exploreRegion(BWTA::Region* region){
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
}

int ScoutManager::newGoal() const {
	return scoutGoals.size();
}


pGoal ScoutManager::getGoal(){
	pGoal p=scoutGoals.front();
	scoutGoals.pop_front();
	return p;
}

