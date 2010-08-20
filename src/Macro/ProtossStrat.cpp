#include <ProtossStrat.h>

ProtossStrat::ProtossStrat():MacroManager()
{
	//Broodwar->printf("Protoss Macro Manager is used.");
}

ProtossStrat::~ProtossStrat()
{
}

void ProtossStrat::eRush(){
	this->buildOrderManager->build(2,BWAPI::UnitTypes::Protoss_Gateway,94);
	this->buildOrderManager->build(5,BWAPI::UnitTypes::Protoss_Zealot,92);
}

void ProtossStrat::onStart(){
	MacroManager::onStart();
	this->buildOrderManager->buildAdditional(4,BWAPI::UnitTypes::Protoss_Probe,100);
	this->buildOrderManager->build(1,BWAPI::UnitTypes::Protoss_Pylon,98);
	this->buildOrderManager->buildAdditional(2,BWAPI::UnitTypes::Protoss_Probe,96);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Gateway,94);
	this->buildOrderManager->buildAdditional(2,BWAPI::UnitTypes::Protoss_Probe,92);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Assimilator,90);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Probe,88);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Cybernetics_Core,86);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Probe,84);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Gateway,82);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Probe,80);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Dragoon,78);
	this->buildOrderManager->buildAdditional(1,BWAPI::UnitTypes::Protoss_Pylon,76);
	this->buildOrderManager->upgrade(1,BWAPI::UpgradeTypes::Singularity_Charge, 74);
	this->buildOrderManager->build(40,BWAPI::UnitTypes::Protoss_Dragoon, 20);

	setScoutTime();
	//this->buildOrderManager->enableDependencyResolver();
}

void ProtossStrat::setScoutTime(){
	BWAPI::Broodwar->printf("%s", BWAPI::Broodwar->mapFileName().c_str());
	if(BWAPI::Broodwar->mapFileName() == "(2)Astral Balance.scm"){
		BWAPI::Broodwar->printf(" ok ");
	}
}

void ProtossStrat::update(){

	this->createProdBuildings();
	if(this->buildManager->getCompletedCount(BWAPI::UnitTypes::Protoss_Dragoon)>=4){
		if(!this->workerManager->autoBuild){
			BWAPI::Broodwar->printf("Auto building probes");
			this->workerManager->enableAutoBuild();
		}
	}

/*
//This must be uncommented if we want the bot to expand. Yet expanding twice is not possible for the moment.
	if(this->shouldExpand() && !expanding ){
		this->expanding = true;
		BWAPI::Broodwar->printf("Expanding at the nearest location");
		this->baseManager->expand(this->naturalExpand,80);
	}
	BWAPI::Broodwar->drawCircleMap(naturalExpand->getPosition().x(),naturalExpand->getPosition().y(),20,BWAPI::Colors::Green, true);
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
	
	bool allGatesFull = true;
	const list<Unit*>& buildings = prodBuildings[UnitTypes::Protoss_Gateway];
	for( list<Unit*>::const_iterator it = buildings.begin(); it != buildings.end(); it++)
		if(!(*it)->isCompleted() || !(*it)->isTraining())
		{
			allGatesFull = false;
			break;
		}
	if( allGatesFull && !underConstruction[UnitTypes::Protoss_Gateway])
	{
		this->buildOrderManager->buildAdditional(1,UnitTypes::Protoss_Gateway,20);
		underConstruction[UnitTypes::Protoss_Gateway] = true;
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
	if( canCreateTechBuildings( UnitTypes::Protoss_Cybernetics_Core, UnitTypes::Protoss_Gateway, 2))
	{
		this->buildOrderManager->buildAdditional(1,UnitTypes::Protoss_Cybernetics_Core,70);
		techBuildings.insert( UnitTypes::Protoss_Cybernetics_Core);
	}
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

void ProtossStrat::initWantedUnits()
{
	wantedUnits[UnitTypes::Protoss_Zealot].plannedType = TYPE_NUMBER;
	wantedUnits[UnitTypes::Protoss_Zealot].plannedValue = 1;
	wantedUnits[UnitTypes::Protoss_Dragoon].plannedType = TYPE_RATIO;
	wantedUnits[UnitTypes::Protoss_Dragoon].plannedValue = 1;
	wantedUnits[UnitTypes::Protoss_High_Templar].plannedType = TYPE_NUMBER;
	wantedUnits[UnitTypes::Protoss_High_Templar].plannedValue = 5;
	wantedUnits[UnitTypes::Protoss_Observer].plannedType = TYPE_NUMBER;
	wantedUnits[UnitTypes::Protoss_Observer].plannedValue = 2;
}
