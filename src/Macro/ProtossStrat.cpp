#include <PrecompiledHeader.h>
#include <ProtossStrat.h>
#include <UnitGroupManager.h>

ProtossStrat::ProtossStrat()
: MacroManager()
, _launchedUpgrades(false)
, _activeBases(0)
{
	//Broodwar->printf("Protoss Macro Manager is used.");
}

ProtossStrat::~ProtossStrat()
{
}

void ProtossStrat::eRush()
{
#ifdef __DEBUG__
    Broodwar->printf("anti rush ON");
#endif
	this->buildOrderManager->build(2,BWAPI::UnitTypes::Protoss_Gateway,94);
	this->buildOrderManager->build(3,BWAPI::UnitTypes::Protoss_Zealot,92);
	this->buildOrderManager->build(2,BWAPI::UnitTypes::Protoss_Pylon,93);
	if (this->workerManager->autoBuild)
    {
		this->workerManager->disableAutoBuild();
	}
}

void ProtossStrat::onStart()
{
	int i = 0;
	for (i=0; i< 12; i++)
    {
		this->needed[i] = 0;
		this->priority[i] = 0;
	}
	MacroManager::onStart();
	this->buildOrderManager->buildAdditional(4,BWAPI::UnitTypes::Protoss_Probe,100);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Pylon,98);
	this->buildOrderManager->buildAdditional(2,BWAPI::UnitTypes::Protoss_Probe,96);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Gateway,94);
	this->buildOrderManager->buildAdditional(2,BWAPI::UnitTypes::Protoss_Probe,92);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Assimilator,90);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Probe,88);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Cybernetics_Core,86);
	//this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Probe,84);
    this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Dragoon,82); /////////////
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Probe,80);
    this->buildOrderManager->buildAdditional(2, BWAPI::UnitTypes::Protoss_Dragoon, 78);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Pylon,76);
	this->buildOrderManager->upgrade(1,BWAPI::UpgradeTypes::Singularity_Charge, 74);
    this->buildOrderManager->buildAdditional(2,BWAPI::UnitTypes::Protoss_Gateway,73);
    this->buildOrderManager->build(6,BWAPI::UnitTypes::Protoss_Dragoon,72);
	if (!this->workerManager->autoBuild)
    {
        //this->workerManager->setAutoBuildPriority(98);
		this->workerManager->enableAutoBuild();
	}
    this->buildOrderManager->build(24, BWAPI::UnitTypes::Protoss_Probe, 95);
	setScoutTime();
	this->buildOrderManager->enableDependencyResolver();
    this->workerManager->setWorkersPerGas(2);
}

void ProtossStrat::setScoutTime()
{
	std::string s = BWAPI::Broodwar->mapFileName();
    BWAPI::Broodwar->printf("%s", s.c_str());
	if (s == "ICCup andromeda 1.0.scx" || s == "ICCup andromeda1.0_ob.scx")
    {
		this->firstScout = 20;

	} else if (s == "ICCup destination 1.1.scx" || s =="ICCup destination 1.1_ob.scx")
    {
		this->firstScout = 16;
	} else if (s == "iCCup heartbreak ridge1.1.scx" || s == "iCCup heartbreak ridge1.1ob.scx" )
    {
		this->firstScout = 16;
	} else if (s == "ICCup python 1.3.scx" || s == "ICCup python 1.3_ob.scm")
    {
		this->firstScout = 20;
	} else if (s == "ICCup tau cross.scx" || s == "ICCup tau cross_ob.scx")
    {
		this->firstScout = 20;
	}
}

void ProtossStrat::update()
{
    if (Broodwar->getFrameCount() > 700 && this->workerManager->autoBuild)  // provisory
    {
        const std::set<Base*>& bases = baseManager->getAllBases();
        unsigned int neededWorkers = 0;
        for( std::set<Base*>::const_iterator it = bases.begin(); it != bases.end(); it++)
        {
            neededWorkers += 3*(*it)->getMinerals().size() + 3*(*it)->getGeysers().size();
        }
        neededWorkers += 2;
        if (workerManager->workers.size() < neededWorkers && SelectAll(UnitTypes::Protoss_Probe).size() < 70)
        {
            for (std::set<BWAPI::Unit*>::iterator it = nexuses.begin(); it != nexuses.end(); ++it)
            {
                if (!(*it)->isTraining() && Broodwar->self()->minerals() >= 50)
                    (*it)->train(UnitTypes::Protoss_Probe);
            }
        }
    } // pallie au bug de ProductionManager/Arbitrator

    if (Broodwar->getFrameCount() > 3200) // provisory
    {
        this->workerManager->setWorkersPerGas(3);
        for (std::set<BWAPI::Unit*>::iterator it = gateways.begin(); it != gateways.end(); ++it) // provisory
        {
            if (!(*it)->isTraining())
            {
                if (Broodwar->self()->gas() >= 250 && Broodwar->self()->minerals() >= 350)
                    (*it)->train(UnitTypes::Protoss_Dragoon);
                else if (SelectAll(UnitTypes::Protoss_Zealot).size() < 20 && ((!this->shouldExpand() && Broodwar->self()->minerals() >= 300) || Broodwar->self()->minerals() >= 500))
                    (*it)->train(UnitTypes::Protoss_Zealot);
            }
        }
    }

    if (Broodwar->getFrameCount() > 7000) // a little less than 5 minutes
    {
        if (!this->workerManager->autoBuild)
        {
            this->workerManager->enableAutoBuild();
        }
        if (Broodwar->getFrameCount() == 7001)
            this->workerManager->setWorkersPerGas(3);
    } else
    {
        if (Broodwar->self()->gas() >= 170 && Broodwar->self()->minerals() <= 100)
        {
            this->workerManager->setWorkersPerGas(2); // TODO TOCHANGE TOURNAMENT3
        }
    }

	this->createProdBuildings();

	if (this->shouldExpand() && !expanding)
    {
		this->expanding = true;
        this->workerManager->setWorkersPerGas(3);
		if (this->baseManager->getBase(this->baseManager->naturalExpand) == NULL)
        {
			this->baseManager->expand(this->baseManager->naturalExpand,82);
		} 
        else 
        {
			this->baseManager->expand(80);
		}
	}
	
	this->buildUnits();	

    if (!(Broodwar->getFrameCount() % 100)) // 24
    {
        this->buildOrderManager->build(this->needed[Dragoon],BWAPI::UnitTypes::Protoss_Dragoon,this->priority[Dragoon]);
        //if (Broodwar->self()->gas() < 50 && this->productionManager->getPlannedCount(UnitTypes::Protoss_Zealot) < 8)
        this->buildOrderManager->build(this->needed[Zealot],BWAPI::UnitTypes::Protoss_Zealot,this->priority[Zealot]);
        //this->buildOrderManager->build(this->needed[Arbiter],BWAPI::UnitTypes::Protoss_Arbiter,this->priority[Arbiter]);
        this->buildOrderManager->build(this->needed[Archon],BWAPI::UnitTypes::Protoss_Archon,this->priority[Archon]);
        //this->buildOrderManager->build(this->needed[Carrier],BWAPI::UnitTypes::Protoss_Carrier,this->priority[Carrier]);
        //this->buildOrderManager->build(this->needed[Corsair],BWAPI::UnitTypes::Protoss_Corsair,this->priority[Corsair]);
        //this->buildOrderManager->build(this->needed[Dark_Archon],BWAPI::UnitTypes::Protoss_Dark_Archon,this->priority[Dark_Archon]);
        //this->buildOrderManager->build(this->needed[Dark_Templar],BWAPI::UnitTypes::Protoss_Dark_Templar,this->priority[Dark_Templar]);
        this->buildOrderManager->build(this->needed[High_Templar],BWAPI::UnitTypes::Protoss_High_Templar,this->priority[High_Templar]);
        this->buildOrderManager->build(this->needed[Observer],BWAPI::UnitTypes::Protoss_Observer,this->priority[Observer]);
        this->buildOrderManager->build(this->needed[Reaver],BWAPI::UnitTypes::Protoss_Reaver,this->priority[Reaver]);
        //this->buildOrderManager->build(this->needed[Scout],BWAPI::UnitTypes::Protoss_Scout,this->priority[Scout]);
        //this->buildOrderManager->build(this->needed[Shuttle],BWAPI::UnitTypes::Protoss_Shuttle,this->priority[Shuttle]);
    }
    if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
    {
        if (Broodwar->self()->gas() >= 50 && Broodwar->self()->minerals() >= 125) // && this->productionManager->getPlannedCount(UnitTypes::Protoss_Dragoon) < (int)(4*this->baseManager->getActiveBases().size()))
        {
            if (this->buildOrderManager->getPlannedCount(UnitTypes::Protoss_Dragoon < 96))
                this->buildOrderManager->buildAdditional(1, UnitTypes::Protoss_Dragoon, 40);
            //this->productionManager->train(UnitTypes::Protoss_Dragoon);
        }
    }
    _activeBases = this->baseManager->getActiveBases().size();
}

void ProtossStrat::buildUnits()
{
	//BO with templar goon zealot
	this->needed[Dragoon] = 60;
	this->priority[Dragoon] = 40;
    this->needed[Zealot] = 7;
    if (Broodwar->enemy()->getRace() == Races::Zerg)
        this->priority[Zealot] = 41;
    else
    {
        if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Leg_Enhancements))
            this->priority[Zealot] = 40;
        else
            this->priority[Zealot] = 30;
    }
	if (this->baseManager->getActiveBases().size() > 1)
    {
        if (!_launchedUpgrades && Broodwar->self()->getUpgradeLevel(UpgradeTypes::Protoss_Ground_Weapons) < 3)
        {
            this->buildOrderManager->build(1, UnitTypes::Protoss_Forge, 50);
            this->buildOrderManager->upgrade(3, UpgradeTypes::Protoss_Ground_Weapons, 43);
            this->buildOrderManager->upgrade(3, UpgradeTypes::Protoss_Ground_Armor, 43);
            this->buildOrderManager->upgrade(3, UpgradeTypes::Protoss_Plasma_Shields, 43);
            _launchedUpgrades = true;
        }
        if (!Broodwar->self()->hasResearched(TechTypes::Psionic_Storm))
            this->buildOrderManager->research(TechTypes::Psionic_Storm, 92);
        this->needed[High_Templar] = 12; // TOCHANGE
        this->priority[High_Templar] = 42;
        this->buildOrderManager->build(3, UnitTypes::Protoss_High_Templar, 51);
        if (!Broodwar->self()->getUpgradeLevel(UpgradeTypes::Leg_Enhancements))
            this->buildOrderManager->upgrade(1, UpgradeTypes::Leg_Enhancements, 70);
	}
    if (this->baseManager->getActiveBases().size() > 3)
    {
        if (this->buildManager->getPlannedCount(UnitTypes::Protoss_Templar_Archives) < 1)
            this->buildOrderManager->build(1, UnitTypes::Protoss_Templar_Archives, 90);
        this->buildOrderManager->build(2, UnitTypes::Protoss_Forge, 50);
        this->priority[High_Templar] = 50;
    }

/*
	//BO with templar goon zealot
	this->needed[Dragoon]++;
	this->priority[Dragoon] = 40;
    this->needed[Zealot] = 12; // 24
    this->priority[Zealot] = 38; // 40
    //if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Protoss_Ground_Weapons) < 3)
    //    this->buildOrderManager->upgrade(3, UpgradeTypes::Protoss_Ground_Weapons, 70);
	if(this->baseManager->getActiveBases().size() > 1)
    {
        /*if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Protoss_Ground_Weapons) < 1)
            this->buildOrderManager->upgrade(1, UpgradeTypes::Protoss_Ground_Weapons, 70);
        if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Protoss_Ground_Armor) < 1)
            this->buildOrderManager->upgrade(1, UpgradeTypes::Protoss_Ground_Armor, 69);
        for (std::set<Base*>::const_iterator it = this->baseManager->getActiveBases().begin();
            it != baseManager->getActiveBases().end(); ++it)
        {
            this->buildOrderManager->build(2, UnitTypes::Protoss_Photon_Cannon, 60, (*it)->getBaseLocation()->getTilePosition());
        }
        if (!Broodwar->self()->hasResearched(TechTypes::Psionic_Storm))
            this->buildOrderManager->research(TechTypes::Psionic_Storm, 70);
		if(this->buildOrderManager->hasResources(BWAPI::UnitTypes::Protoss_High_Templar)){
			this->needed[High_Templar]++;
			this->priority[High_Templar] = 40;
			//BWAPI::Broodwar->printf("one more now : %d", this->needed[High_Templar]); //////////////
*/
}

void ProtossStrat::buildGeyser()
{
	/*
	std::set<Base*>& bases = baseManager->getActiveBases();
	for( set<Base*>::iterator it = bases.begin(); it != bases.end(); it++)
	{
		if( ((*it)->getGeysers().size() - (*it)->exploitedGeysers) && //Geyser remaining
			buildManager->getCompletedCount( UnitTypes::Protoss_Gateway) > 1 )
		{
			this->buildOrderManager->build(1,UnitTypes::Protoss_Assimilator,60);
			(*it)->exploitedGeysers++; // TODO enlever les geyser detruits.
		}
	}
	*/
}

void ProtossStrat::createProdBuildings()
{
	buildGates();
	//buildStarGates();
}

void ProtossStrat::buildGates()
{	
    if (this->gateways.empty())
        return;
    //if (this->baseManager->getActiveBases().size() > 2 && _activeBases - this->baseManager->getActiveBases().size() != 0)
    //    this->buildOrderManager->build(3*this->baseManager->getActiveBases().size(), BWAPI::UnitTypes::Protoss_Gateway, 40);
    if (this->baseManager->getActiveBases().size() > 1)
    {
        bool allGatesFull = true;
        for (std::set<Unit*>::const_iterator it = gateways.begin(); it != gateways.end(); it++)
            if (!(*it)->isCompleted() || !(*it)->isTraining())
            {
                allGatesFull = false;
                break;
            }
            if (allGatesFull && Broodwar->self()->minerals() >= 150 && this->buildManager->getPlannedCount(UnitTypes::Protoss_Gateway) < (int)gateways.size() + 1)
            {
                this->buildOrderManager->buildAdditional(1,UnitTypes::Protoss_Gateway, 50);
            }
    }
}

void ProtossStrat::createTechBuildings()
{
	/*
	if( canCreateTechBuildings( UnitTypes::Protoss_Forge, UnitTypes::Protoss_Gateway, 1))
	{
		this->buildOrderManager->buildAdditional(1,UnitTypes::Protoss_Forge,70);
		techBuildings.insert( UnitTypes::Protoss_Forge);
	}
	*/
	/*if (canCreateTechBuildings( UnitTypes::Protoss_Cybernetics_Core, UnitTypes::Protoss_Gateway, 2))
	{
		this->buildOrderManager->buildAdditional(1,UnitTypes::Protoss_Cybernetics_Core,70);
		techBuildings.insert(UnitTypes::Protoss_Cybernetics_Core);
	}*/
	/*
	if( canCreateTechBuildings( UnitTypes::Protoss_Citadel_of_Adun, UnitTypes::Protoss_Cybernetics_Core, 1))
	{
	this->buildOrderManager->buildAdditional(1,UnitTypes::Protoss_Citadel_of_Adun,70);
	techBuildings.insert( UnitTypes::Protoss_Citadel_of_Adun);
	}

	if( canCreateTechBuildings( UnitTypes::Protoss_Templar_Archives, UnitTypes::Protoss_Citadel_of_Adun, 1))
	{
	this->buildOrderManager->buildAdditional(1,UnitTypes::Protoss_Templar_Archives,70);
	techBuildings.insert( UnitTypes::Protoss_Templar_Archives);
	}
	*/
}

void ProtossStrat::buildDefenses()
{
	/*
	return;
	if( canCreateDefenseBuildings( UnitTypes::Protoss_Photon_Cannon, UnitTypes::Protoss_Forge))
	{
		this->buildOrderManager->buildAdditional(1,UnitTypes::Protoss_Photon_Cannon,70);
		underConstruction[UnitTypes::Protoss_Photon_Cannon]++;
	}
	*/
}

void ProtossStrat::someAir()
{
    if (Broodwar->enemy()->getRace() == Races::Zerg)
    {
        buildOrderManager->build(8, UnitTypes::Protoss_Corsair, 42);
    }
}