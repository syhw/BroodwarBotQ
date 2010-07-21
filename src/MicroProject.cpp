#include "MicroProject.h"
#include "Goal.h"
#include "FormationSubgoal.h"
#include <stdio.h>
#include <QtGui/QApplication.h>
using namespace BWAPI;
using namespace std;

void MicroAIModule::onStart()
{
	///Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
	// Enable some cheat flags
    Broodwar->printf("ON START !!\n");
	Broodwar->enableFlag(Flag::UserInput);
    //Broodwar->setLocalSpeed(0);
	//Broodwar->enableFlag(Flag::CompleteMapInformation);
	BWTA::readMap();
	BWTA::analyze();
    this->eUnitsFilter = & EUnitsFilter::Instance();
    this->mapManager = & MapManager::Instance();
	this->objectManager = & ObjectManager::Instance();
    this->regions = & Regions::Instance();

	mm = new UnitsGroup();
    mD = new UnitsGroup();

	// Vec center;
	std::set<Unit*> allUnits = Broodwar->getAllUnits();

	for (std::set<Unit*>::iterator i=allUnits.begin(); i!=allUnits.end(); i++)
	{
        onUnitShow(*i);
        if ((*i)->getPlayer() != Broodwar->self())
            continue;
		if ((*i)->getType().isBuilding())
		{
			this->buildings.insert(std::make_pair(*i,(*i)->getType()));
            regions->onUnitCreate(*i);
            mapManager->onUnitCreate(*i);
		} 
        else if ((*i)->getType() == BWAPI::UnitTypes::Protoss_Zealot)
		{
            // Broodwar->printf("Took control of: %s\n", (*i)->getType().getName().c_str() );
			mm->takeControl(*i);
		}
        else if ((*i)->getType() != BWAPI::UnitTypes::Protoss_Zealot)
        {
            mD->takeControl(*i);
        }
	}

	/* std::set<Player*> players=Broodwar->getPlayers();
	Broodwar->printf("The following players are in this replay:");
	for(std::set<Player*>::iterator p=players.begin();p!=players.end();p++)
	{
	if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
	{
	Broodwar->printf("%s, playing as a %s",(*p)->getName().c_str(),(*p)->getRace().getName().c_str());
	}
	} */

	/* Broodwar->printf("The match up is %s v %s",
	Broodwar->self()->getRace().getName().c_str(),
	Broodwar->enemy()->getRace().getName().c_str()); */

	//*
	std::list<pGoal> goals;
	std::list<pGoal> goals2;
	// goals.push_back(Formation(0,1,Position(1000,1000)); 
    /// TEST test_path with tanks & vultures: 
    /// goals.push_back(new Goal("direct line to 1120, 704", new Formation(Vec(1120, 704), Vec(-0.5, -0.5))));
    /// TEST test_square
    /// goals.push_back(Goal("direct square 480, 1600", new SquareFormation(Vec(480, 1600))));
    /// TEST test_pathfinding
    /// goals.push_back(pGoal(new Goal("direct square 384, 768", pFormation(new SquareFormation(Vec(60*32, 61*32))))));

	BWAPI::TilePosition mp = Broodwar->self()->getStartLocation();
	Position p;
	for(std::set<BWAPI::TilePosition>::iterator l=Broodwar->getStartLocations().begin(); l!=Broodwar->getStartLocations().end(); l++)
	{
		if (!((*l) == mp))
			p = BWAPI::Position(*l);
	}

    /// EXEMPLE FOR THE FLOCK_2 and FLOCK_8 MAPS
  /*  pFormation tmp_form = pFormation(new SquareFormation(Position(56*32,56*32), Vec(1,0)));
	pSubgoal tmp_subgoal = pSubgoal(new FormationSubgoal(
        SL_AND, tmp_form, mm)
    );
    pGoal tmp_goal = pGoal(new Goal(tmp_subgoal));
    goals.push_back(tmp_goal);
*/
    /// TEST PATHFINDING
    /*
    pFormation tmp_form = pFormation(new LineFormation(Position(101*32,62*32), Vec(1,0)));
    pSubgoal tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
    pGoal tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
    goals.push_back(tmp_goal);
*/
    
    pFormation tmp_form = pFormation(new LineFormation(Position(29*32,8*32), Vec(1,0)));
    pSubgoal tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
    pGoal tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
    goals.push_back(tmp_goal);

    tmp_form = pFormation(new LineFormation(Position(35*32,8*32), Vec(1,0)));
    tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
    tmp_goal = pGoal(new Goal(mD, tmp_subgoal));
    goals2.push_back(tmp_goal);

  // tmp_form = pFormation(new LineFormation(Position(33*32,9*32), Vec(1,0)));
  //  tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form, mm));
  //  tmp_goal = pGoal(new Goal(tmp_subgoal));
  //  goals.push_back(tmp_goal);
	 
    //gl->setFormation(form);

	//goals.push_back(Formation(0,1,Position(400,0)));
	//goals.push_back(Goal("attack move right !!!!!!!", Position(1999,1000)));
	//goals->push_back(new Goal("attack move just there", Position(500,1300)));
	//goals->push_back(new Goal());
	mm->setGoals(goals);
    mD->setGoals(goals2);
	/*/
	mm->setGoals(Goal("formation line x+400", new LineFormation(center, Vec(1, 3))));
	Broodwar->printf( "size: %i", mm->goals.size());
	Broodwar->printf( "center: %f, %f", mm->goals.front().formation->center.x, mm->goals.front().formation->center.y);
	//*/

#ifdef BW_QT_DEBUG
	g_onStartDone = true;
#endif
}

void MicroAIModule::onFrame()
{
#ifdef BW_QT_DEBUG
    if (!*qapp)
        Broodwar->printf("Qt not connected\n");
#endif
    objectManager->onFrame();
	if (mm != NULL) 
        mm->update();
	if (mD != NULL) 
        mD->update();
    regions->display();
	/*if (Broodwar->getFrameCount()%300==0)
	{
	//Every 300 frames we will print some basic unit stats
	showStats();
	}

	//we will visualize the chokepoints with red lines
	for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
	{
	for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();c++)
	{
	Position point1=(*c)->getSides().first;
	Position point2=(*c)->getSides().second;
	Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Red);
	}
	}*/

	//if (mm != NULL) mm->display();

    //we will iterate through all the regions and draw the polygon outline of it in green.
	for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
	{
		BWTA::Polygon p=(*r)->getPolygon();
		for(int j=0;j<(int)p.size();j++)
		{
			Position point1=p[j];
			Position point2=p[(j+1) % p.size()];
			Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Green);
		}
	}
    //mapManager->drawBuildings();
    //mapManager->drawWalkability();
}

MicroAIModule::~MicroAIModule()
{
#ifdef BW_QT_DEBUG
    (*qapp)->quit();
#endif
    MapManager::Destroy();
		ObjectManager::Destroy();
    Regions::Destroy();
    mm->~UnitsGroup();
    mD->~UnitsGroup();
}

#ifdef BW_QT_DEBUG
MicroAIModule::MicroAIModule(QApplication** qapplication)
{
    qapp = qapplication;
}
#else
MicroAIModule::MicroAIModule()
{
}
#endif

/* void MicroAIModule::onRemove(BWAPI::Unit* unit)
{
BWAPI::Broodwar->printf("just entered onRemove()");
int a = 69;
for(int i = 0; i < 5000000; i++) a = 42;
int b = a;
//this will erase the unit from our set of buildings, if it is in the set.
this->buildings.erase(unit);
} */

void MicroAIModule::onEnd(bool isWinner)
{
	if (isWinner)
	{
		//log win to file
	}
}

bool MicroAIModule::onSendText(std::string text)
{
	if (text=="/show stats")
	{
		showStats();
		return false;
	} else if (text=="/show players")
	{
		showPlayers();
		return false;
	} else if (text=="/show forces")
	{
		showForces();
		return false;
	} else if (text=="/analyze")
	{
		if (analyzed == false)
		{
			Broodwar->printf("Analyzing map... this may take a minute");
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
		}
		return false;
    } else if (text=="/euf") // eUnitsFilter
    {
        eUnitsFilter->bwOutput();
    } else if (text.substr(0,3) == "/go")
    {
        int x = 0;
        int y = 0;
        if (text[3] != ' ')
            Broodwar->printf("Usage: /go XCOORD YCOORD;\n");
        int lx = text.find(' ', 4) - 4;
        int ly = text.find(';') - (4+lx);
        std::string xx = text.substr(4, 4+lx);
        x = atoi(xx.c_str());
        std::string yy = text.substr(4+lx+1, 4+lx+ly);
        y = atoi(yy.c_str());
        Broodwar->printf("lx: %d | ly: %d", lx, ly);
        set<pBayesianUnit> tmp;
        mm->selectedUnits(tmp);
        for (set<pBayesianUnit>::iterator it = tmp.begin(); it != tmp.end(); ++it)
        {
            (*it)->target = Position(x, y);
        }
        Broodwar->printf("We will go in %d, %d\n", x, y);
	} else
	{
		Broodwar->printf("You typed '%s'!",text.c_str());
	}
	return true;
}

void MicroAIModule::onUnitCreate(Unit* unit)
{
    mapManager->onUnitCreate(unit);
    regions->onUnitCreate(unit);
}

void MicroAIModule::onUnitDestroy(Unit* unit)
{
    eUnitsFilter->onUnitDestroy(unit);
    mapManager->onUnitDestroy(unit);
    regions->onUnitDestroy(unit);
    mm->onUnitDestroy(unit);
}

void MicroAIModule::onUnitShow(Unit* unit)
{
    eUnitsFilter->onUnitShow(unit);
    mapManager->onUnitShow(unit);
	regions->onUnitShow(unit);
    mm->onUnitShow(unit);
}
void MicroAIModule::onUnitHide(Unit* unit)
{
    eUnitsFilter->onUnitHide(unit);
    mapManager->onUnitHide(unit);
	regions->onUnitHide(unit);
    mm->onUnitHide(unit);
}

DWORD WINAPI AnalyzeThread()
{
	BWAPI::Broodwar->printf("just entered AnalyseThread()");
	int a = 69;
	for(int i = 0; i < 50000000; i++) a = 42;
	int b = a;
	BWTA::analyze();
	analyzed   = true;
	analysis_just_finished = true;
	home       = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
	enemy_base = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
	return 0;
}

void MicroAIModule::showStats()
{
	std::set<Unit*> myUnits = Broodwar->self()->getUnits();
	Broodwar->printf("I have %d units:",myUnits.size());
	std::map<UnitType, int> unitTypeCounts;
	for(std::set<Unit*>::iterator i=myUnits.begin();i!=myUnits.end();i++)
	{
		if (unitTypeCounts.find((*i)->getType())==unitTypeCounts.end())
		{
			unitTypeCounts.insert(std::make_pair((*i)->getType(),0));
		}
		unitTypeCounts.find((*i)->getType())->second++;
	}
	for(std::map<UnitType,int>::iterator i=unitTypeCounts.begin();i!=unitTypeCounts.end();i++)
	{
		Broodwar->printf("- %d %ss",(*i).second, (*i).first.getName().c_str());
	}
}

void MicroAIModule::showPlayers()
{
	std::set<Player*> players=Broodwar->getPlayers();
	for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
	{
		Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
	}
}
void MicroAIModule::showForces()
{
	std::set<Force*> forces=Broodwar->getForces();
	for(std::set<Force*>::iterator i=forces.begin();i!=forces.end();i++)
	{
		std::set<Player*> players=(*i)->getPlayers();
		Broodwar->printf("Force %s has the following players:",(*i)->getName().c_str());
		for(std::set<Player*>::iterator j=players.begin();j!=players.end();j++)
		{
			Broodwar->printf("  - Player [%d]: %s",(*j)->getID(),(*j)->getName().c_str());
		}
	}
}
