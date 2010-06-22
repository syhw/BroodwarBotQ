#include "MacroProject.h"
#include <Util.h>
#include <time.h>

#include <UnitsGroup.h>

using namespace BWAPI;

BattleBroodAI::BattleBroodAI()
{
    clearLog();

    std::set<UnitType>& aut = BWAPI::UnitTypes::allUnitTypes();
    for (std::set<UnitType>::iterator wu = aut.begin(); wu != aut.end(); wu++)
        log( "%s requieres %i %s", wu->getName().c_str(), wu->whatBuilds().second, wu->whatBuilds().first.getName().c_str());

    log( "\n\n\n\n\n");

    for (std::set<UnitType>::iterator wu = aut.begin(); wu != aut.end(); wu++)
        for(std::map<UnitType, int >::const_iterator it = wu->requiredUnits().begin(); it != wu->requiredUnits().end(); it++)
            log("create %s requieres %s", wu->getName().c_str(), it->first.getName().c_str());

    log( "\n\n\n\n\n");

    for (std::set<UnitType>::iterator wu = aut.begin(); wu != aut.end(); wu++)
        if( wu->isBuilding() && !wu->isResourceDepot() && wu->canProduce())
            log( "%s is a prod building.", wu->getName().c_str());

    log( "\n\n\n\n\n");

    for (std::set<UnitType>::iterator wu = aut.begin(); wu != aut.end(); wu++)
        if( wu->isBuilding() && !wu->isResourceDepot() && !wu->canProduce() && wu->canAttack())
            log( "%s is a def building.", wu->getName().c_str());

    log( "\n\n\n\n\n");

    for (std::set<UnitType>::iterator wu = aut.begin(); wu != aut.end(); wu++)
        if( !wu->isBuilding() && !wu->isWorker())
            log( "%s is a unit.", wu->getName().c_str());

    log( "\n\n\n\n\n");

    for (std::set<UnitType>::iterator wu = aut.begin(); wu != aut.end(); wu++)
        if( !wu->isBuilding() && !wu->isWorker())
            log( "%s has a range of %i.", wu->getName().c_str(), wu->seekRange());
}

BattleBroodAI::~BattleBroodAI()
{
    TimeManager::Destroy();
    Arbitrator::Arbitrator<BWAPI::Unit*,double>::Destroy();
    BuildManager::Destroy();
    TechManager::Destroy();
    UpgradeManager::Destroy();
    BuildOrderManager::Destroy();
    ScoutManager::Destroy();
    WorkerManager::Destroy();
    BaseManager::Destroy();
    SupplyManager::Destroy();
    MicroManager::Destroy();
    Regions::Destroy();
    MapManager::Destroy();
    EUnitsFilter::Destroy();


    if( Broodwar->self()->getRace() == Races::Protoss)
        ProtossStrat::Destroy();
    else if( Broodwar->self()->getRace() == Races::Terran)
        TerranStrat::Destroy();
    else if( Broodwar->self()->getRace() == Races::Zerg)
        ZergStrat::Destroy();
}

void BattleBroodAI::onStart()
{
    _CrtDumpMemoryLeaks();      // anti-memory leaks
	Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
	// Enable some cheat flags
	Broodwar->enableFlag(Flag::UserInput);
	// Uncomment to enable complete map information
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Speed up the game to the maximum
    Broodwar->setLocalSpeed(0);

	this->showManagerAssignments=false;

	if (Broodwar->isReplay())
	{
		Broodwar->printf("The following players are in this replay:");
		for(std::set<Player*>::iterator p=Broodwar->getPlayers().begin();p!=Broodwar->getPlayers().end();p++)
		{
			if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
			{
				Broodwar->printf("%s, playing as a %s",(*p)->getName().c_str(),(*p)->getRace().getName().c_str());
			}
		}
		return;
	}

	// Enable some cheat flags
	//Broodwar->enableFlag(Flag::UserInput);
	//Broodwar->enableFlag(Flag::CompleteMapInformation);
	BWTA::readMap();
	BWTA::analyze();
	this->analyzed=true;
	this->timeManager       = & TimeManager::Instance();
	this->arbitrator		= & Arbitrator::Arbitrator<BWAPI::Unit*,double>::Instance();
	this->buildManager      = & BuildManager::Instance();
	this->techManager       = & TechManager::Instance();
	this->upgradeManager    = & UpgradeManager::Instance();
	this->buildOrderManager = & BuildOrderManager::Instance();
	this->scoutManager      = & ScoutManager::Instance();
	this->workerManager     = & WorkerManager::Instance();
	this->baseManager       = & BaseManager::Instance();
	this->supplyManager     = & SupplyManager::Instance();
    this->mapManager        = & MapManager::Instance();
    this->eUnitsFilter      = & EUnitsFilter::Instance();

    this->supplyManager->setBuildManager(this->buildManager);
    this->supplyManager->setBuildOrderManager(this->buildOrderManager);
    this->techManager->setBuildingPlacer(this->buildManager->getBuildingPlacer());
    this->upgradeManager->setBuildingPlacer(this->buildManager->getBuildingPlacer());
    this->workerManager->setBaseManager(this->baseManager);
    this->baseManager->setBuildOrderManager(this->buildOrderManager);

    this->baseManager->update();

    if( Broodwar->self()->getRace() == Races::Protoss)
        this->macroManager = & ProtossStrat::Instance();
    else if( Broodwar->self()->getRace() == Races::Terran)
        this->macroManager = & TerranStrat::Instance();
    else if( Broodwar->self()->getRace() == Races::Zerg)
        this->macroManager = & ZergStrat::Instance();

    this->microManager = & MicroManager::Instance();
    this->regions = & Regions::Instance();

    Broodwar->printf("The match up is %s v %s",
        Broodwar->self()->getRace().getName().c_str(),
        Broodwar->enemy()->getRace().getName().c_str());
}

void BattleBroodAI::onEnd(bool isWinner)
{
    if (isWinner)
    {
        //log win to file
    }
}

void BattleBroodAI::onFrame()
{
    //clock_t start = clock();

    // log("IN BBAI::onFrame()");
    ObjectManager::updateOM();
    if (Broodwar->isReplay()) return;
    if (!this->analyzed) return;
    this->timeManager->update();
    this->buildManager->update();
    this->buildOrderManager->update();
    this->baseManager->update();
    this->workerManager->update();
    this->techManager->update();
    this->upgradeManager->update();
    this->macroManager->update();
    this->supplyManager->update();
    this->scoutManager->update();
    this->arbitrator->update();
    ////this->unitManager->update();
    this->microManager->update();
    this->regions->update();

    // Scout example to remove TODO
    if( (Broodwar->getFrameCount() % (100*24)) == 0)
        scoutManager->checkEmptyXP();

    std::set<Unit*> units=Broodwar->self()->getUnits();
    if (this->showManagerAssignments)
    {
        for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++)
        {
            if (this->arbitrator->hasBid(*i))
            {
                int x=(*i)->getPosition().x();
                int y=(*i)->getPosition().y();
                std::list< std::pair< Arbitrator::Controller<BWAPI::Unit*,double>*, double> > bids=this->arbitrator->getAllBidders(*i);
                int y_off=0;
                for(std::list< std::pair< Arbitrator::Controller<BWAPI::Unit*,double>*, double> >::iterator j=bids.begin();j!=bids.end();j++)
                {
                    Broodwar->drawText(CoordinateType::Map,x,y+y_off,"%s: %d",j->first->getName().c_str(),(int)j->second);
                    y_off+=20;
                }
            }
        }
    }
    /*
    for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++)
    {
    int x=(*i)->getPosition().x();
    int y=(*i)->getPosition().y();
    Broodwar->drawCircle(CoordinateType::Map,x,y,(*i)->getType().airWeapon()->maxRange(),Colors::Blue,false);
    Broodwar->drawCircle(CoordinateType::Map,x,y,(*i)->getType().groundWeapon()->maxRange(),Colors::Green,false);
    }
    */

    //we will iterate through all the base locations, and draw their outlines.
    for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
    {
        TilePosition p=(*i)->getTilePosition();
        Position c=(*i)->getPosition();

        //draw outline of center location
        Broodwar->drawBox(CoordinateType::Map,p.x()*32,p.y()*32,p.x()*32+4*32,p.y()*32+3*32,Colors::Blue,false);

        //draw a circle at each mineral patch
        for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getMinerals().begin();j!=(*i)->getMinerals().end();j++)
        {
            Position q=(*j)->getPosition();
            Broodwar->drawCircle(CoordinateType::Map,q.x(),q.y(),30,Colors::Cyan,false);
        }

        //draw the outlines of vespene geysers
        for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getGeysers().begin();j!=(*i)->getGeysers().end();j++)
        {
            TilePosition q=(*j)->getTilePosition();
            Broodwar->drawBox(CoordinateType::Map,q.x()*32,q.y()*32,q.x()*32+4*32,q.y()*32+2*32,Colors::Orange,false);
        }

        //if this is an island expansion, draw a yellow circle around the base location
        if ((*i)->isIsland())
        {
            Broodwar->drawCircle(CoordinateType::Map,c.x(),c.y(),80,Colors::Yellow,false);
        }
    }

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

    //we will visualize the chokepoints with red lines
    for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
    {
        for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();c++)
        {
            Position point1=(*c)->getSides().first;
            Position point2=(*c)->getSides().second;
            Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Red);
        }
    }


    /* // Old AI: to merge !
    if (show_visibility_data)
    {
    for(int x=0;x<Broodwar->mapWidth();x++)
    {
    for(int y=0;y<Broodwar->mapHeight();y++)
    {
    if (Broodwar->isExplored(x,y))
    {
    if (Broodwar->isVisible(x,y))
    Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Green);
    else
    Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Blue);
    }
    else
    Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Red);
    }
    }
    }

    trainDrones();

    if (Broodwar->isReplay())
    return;

    drawStats();
    if (analyzed && Broodwar->getFrameCount()%30==0)
    {
    //order one of our workers to guard our chokepoint.
    for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
    {
    if ((*i)->getType().isWorker())
    {
    //get the chokepoints linked to our home region
    std::set<BWTA::Chokepoint*> chokepoints= home->getChokepoints();
    double min_length=10000;
    BWTA::Chokepoint* choke=NULL;

    //iterate through all chokepoints and look for the one with the smallest gap (least width)
    for(std::set<BWTA::Chokepoint*>::iterator c=chokepoints.begin();c!=chokepoints.end();c++)
    {
    double length=(*c)->getWidth();
    if (length<min_length || choke==NULL)
    {
    min_length=length;
    choke=*c;
    }
    }

    //order the worker to move to the center of the gap
    (*i)->rightClick(choke->getCenter());
    break;
    }
    }
    }
    if (analyzed)
    {
    //we will iterate through all the base locations, and draw their outlines.
    for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
    {
    TilePosition p=(*i)->getTilePosition();
    Position c=(*i)->getPosition();

    //draw outline of center location
    Broodwar->drawBox(CoordinateType::Map,p.x()*32,p.y()*32,p.x()*32+4*32,p.y()*32+3*32,Colors::Blue,false);

    //draw a circle at each mineral patch
    for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getStaticMinerals().begin();j!=(*i)->getStaticMinerals().end();j++)
    {
    Position q=(*j)->getInitialPosition();
    Broodwar->drawCircle(CoordinateType::Map,q.x(),q.y(),30,Colors::Cyan,false);
    }

    //draw the outlines of vespene geysers
    for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getGeysers().begin();j!=(*i)->getGeysers().end();j++)
    {
    TilePosition q=(*j)->getInitialTilePosition();
    Broodwar->drawBox(CoordinateType::Map,q.x()*32,q.y()*32,q.x()*32+4*32,q.y()*32+2*32,Colors::Orange,false);
    }

    //if this is an island expansion, draw a yellow circle around the base location
    if ((*i)->isIsland())
    {
    Broodwar->drawCircle(CoordinateType::Map,c.x(),c.y(),80,Colors::Yellow,false);
    }
    }

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

    //we will visualize the chokepoints with red lines
    for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
    {
    for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();c++)
    {
    Position point1=(*c)->getSides().first;
    Position point2=(*c)->getSides().second;
    Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Red);
    }
    }
    }
    if (analysis_just_finished)
    {
    Broodwar->printf("Finished analyzing map.");
    analysis_just_finished=false;
    }*/
    // log("OUT BBAI::onFrame()");

    display();
    //clock_t end = clock();
    //Broodwar->printf("Iterations took %f", (double)(end-start)/CLOCKS_PER_SEC);
}

void BattleBroodAI::onUnitCreate(BWAPI::Unit* unit)
{
    this->regions->onUnitCreate(unit);
    this->mapManager->onUnitCreate(unit);
    //Broodwar->printf("BBAI::onUnitCreate() %s", unit->getType().getName().c_str());
    //log("IN BBAI::onUnitCreate()");
    macroManager->onUnitCreate( unit);
    //unitManager->onUnitCreate( unit);
    microManager->onUnitCreate(unit);

    if (!Broodwar->isReplay())
    {
        //Broodwar->sendText("A %s [%x] has been created at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
        if(unit->getType().isWorker())
        {
            //Broodwar->printf("New worker created.");
            Unit* closestMineral=NULL;
            for(std::set<Unit*>::iterator m=Broodwar->getMinerals().begin();m!=Broodwar->getMinerals().end();m++)
            {
                if (closestMineral==NULL || unit->getDistance(*m)<unit->getDistance(closestMineral))
                    closestMineral=*m;
            }
            if (closestMineral!=NULL)
            {
                unit->rightClick(closestMineral);	
                //Broodwar->printf("A mineral has been found.");
            }
            //else 
            //	Broodwar->printf("No mineral found.");
        }
        else
        {
            //Broodwar->printf("Unknown unit type.");
        }
    }
    else
    {
        /*if we are in a replay, then we will print out the build order
        (just of the buildings, not the units).*/
        if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
        {
            //Broodwar->printf("Building created.");
            int seconds=Broodwar->getFrameCount()/24;
            int minutes=seconds/60;
            seconds%=60;
            //Broodwar->sendText("%.2d:%.2d: %s creates a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
        }
    }
    // log("OUT BBAI::onUnitCreate()");
}
void BattleBroodAI::onUnitDestroy(BWAPI::Unit* unit)
{
    //if (!Broodwar->isReplay())
    //	Broodwar->sendText("A %s [%x] has been destroyed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());

    this->arbitrator->onRemoveObject(unit);
    this->baseManager->onUnitDestroy(unit);
    this->buildManager->onUnitDestroy(unit);
    this->macroManager->onUnitDestroy( unit);
    this->microManager->onUnitDestroy(unit);
    this->regions->onUnitDestroy(unit);
    this->scoutManager->onUnitDestroy(unit);
    this->techManager->onUnitDestroy(unit);
    this->upgradeManager->onUnitDestroy(unit);
    this->workerManager->onUnitDestroy(unit);
    this->mapManager->onUnitDestroy(unit);
    this->eUnitsFilter->onUnitDestroy(unit);
}

void BattleBroodAI::onUnitMorph(BWAPI::Unit* unit)
{
	//if (!Broodwar->isReplay())
	//	Broodwar->sendText("A %s [%x] has been morphed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	//else
	{
		/*if we are in a replay, then we will print out the build order
		(just of the buildings, not the units).*/
		if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
		{
			int seconds=Broodwar->getFrameCount()/24;
			int minutes=seconds/60;
			seconds%=60;
			//Broodwar->sendText("%.2d:%.2d: %s morphs a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
		}
	}
    eUnitsFilter->update(unit);
}

void BattleBroodAI::onUnitShow(BWAPI::Unit* unit)
{
    //if (!Broodwar->isReplay())
    //	Broodwar->sendText("A %s [%x] has been spotted at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());

    regions->onUnitShow(unit);
    mapManager->onUnitShow(unit);
    eUnitsFilter->update(unit);
}

void BattleBroodAI::onUnitHide(BWAPI::Unit* unit)
{
    //if (!Broodwar->isReplay())
    //	Broodwar->sendText("A %s [%x] was last seen at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());

    regions->onUnitHide(unit);
    mapManager->onUnitHide(unit);
    eUnitsFilter->update(unit);
}

void BattleBroodAI::onUnitRenegade(BWAPI::Unit* unit)
{
	//if (!Broodwar->isReplay())
	//	Broodwar->sendText("A %s [%x] is now owned by %s",unit->getType().getName().c_str(),unit,unit->getPlayer()->getName().c_str());
    eUnitsFilter->update(unit);
}

void BattleBroodAI::onPlayerLeft(BWAPI::Player* player)
{
    //Broodwar->sendText("%s left the game.",player->getName().c_str());
}

void BattleBroodAI::onNukeDetect(BWAPI::Position target)
{
    //if (target!=Positions::Unknown)
    //	Broodwar->printf("Nuclear Launch Detected at (%d,%d)",target.x(),target.y());
    //else
    //	Broodwar->printf("Nuclear Launch Detected");
}

bool BattleBroodAI::onSendText(std::string text)
{
    if (text=="/show players")
    {
        showPlayers();
        return false;
    } else if (text=="/show forces")
    {
        showForces();
        return false;
    } else if (text=="/show visibility")
    {
        show_visibility_data=true;
    } else if (text=="/analyze")
    {
        if (analyzed == false)
        {
            Broodwar->printf("Analyzing map... this may take a minute");
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
        }
        return false;
    }
    else if (text.substr(0,9) == "/setspeed")
    {
        int x = 0;
        if (text[9] != ' ')
            Broodwar->printf("Usage: /setspeed SPEEDVALUE");
        std::string xx = text.substr(10);
        x = atoi(xx.c_str());
        Broodwar->printf("Set Speed %i", x);
        Broodwar->setLocalSpeed(x);
    }
    else if (text=="/target")
    {
        set<pBayesianUnit> tmp;
        for (std::list<UnitsGroup*>::iterator it = this->microManager->unitsgroups.begin(); it != this->microManager->unitsgroups.end(); it++)
        {
            set<pBayesianUnit> tmp;
            (*it)->selectedUnits(tmp);
            if (tmp.size() == 0)
                Broodwar->printf("No selected units");
            for (std::set<pBayesianUnit>::const_iterator i = tmp.begin(); i != tmp.end(); ++i)
            {
                Broodwar->printf("Target de l'unité : (%i, %i)", (*i)->target.x(), (*i)->target.y());
                Position pos(Broodwar->getScreenPosition() + Broodwar->getMousePosition());
                Broodwar->printf("Curseur : (%i, %i)", pos.x(), pos.y());
            }
        }
    }

    UnitType type=UnitTypes::getUnitType(text);
    if (text=="debug")
    {
        this->showManagerAssignments=true;
        return true;
    }
    if (type!=UnitTypes::Unknown)
    {
        this->buildManager->build(type);
    }
    else
    {
        TechType type=TechTypes::getTechType(text);
        if (type!=TechTypes::Unknown)
        {
            this->techManager->research(type);
        }
        else
        {
            UpgradeType type=UpgradeTypes::getUpgradeType(text);
            if (type!=UpgradeTypes::Unknown)
            {
                this->upgradeManager->upgrade(type);
            }
            else
                Broodwar->printf("You typed '%s'!",text.c_str());
        }
    }
    return true;
}

DWORD WINAPI AnalyzeThread()
{
    BWTA::analyze();
    analyzed   = true;
    analysis_just_finished = true;

    //self start location only available if the map has base locations
    if (BWTA::getStartLocation(BWAPI::Broodwar->self())!=NULL)
    {
        home = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
    }
    //enemy start location only available if Complete Map Information is enabled.
    if (BWTA::getStartLocation(BWAPI::Broodwar->enemy())!=NULL)
    {
        enemy_base = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
    }
    return 0;
}

void BattleBroodAI::drawStats()
{
    std::set<Unit*> myUnits = Broodwar->self()->getUnits();
    Broodwar->drawTextScreen(5,0,"I have %d units:",myUnits.size());
    std::map<UnitType, int> unitTypeCounts;
    for(std::set<Unit*>::iterator i=myUnits.begin();i!=myUnits.end();i++)
    {
        if (unitTypeCounts.find((*i)->getType())==unitTypeCounts.end())
        {
            unitTypeCounts.insert(std::make_pair((*i)->getType(),0));
        }
        unitTypeCounts.find((*i)->getType())->second++;
    }
    int line=1;
    for(std::map<UnitType,int>::iterator i=unitTypeCounts.begin();i!=unitTypeCounts.end();i++)
    {
        Broodwar->drawTextScreen(5,16*line,"- %d %ss",(*i).second, (*i).first.getName().c_str());
        line++;
    }
}

void BattleBroodAI::showPlayers()
{
    std::set<Player*> players=Broodwar->getPlayers();
    for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
    {
        Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
    }
}
void BattleBroodAI::showForces()
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

// This method is called from onFrame() and allow to display graphical debug informations on each component.
void BattleBroodAI::display()
{
    regions->display();
    microManager->display();
}
