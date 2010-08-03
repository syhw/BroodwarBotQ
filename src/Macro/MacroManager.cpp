#include <MacroManager.h>
#include <math.h>

MacroManager::MacroManager()
: BaseObject("MacroManager")
, expanding(0)
{
	this->buildOrderManager = NULL;
	this->productionManager = NULL;
	this->buildManager = NULL;
	this->baseManager = NULL;
	this->workerManager = NULL;
}

MacroManager::~MacroManager()
{
}

void MacroManager::setDependencies(BuildOrderManager * bom, ProductionManager * pm, BuildManager * bm,
BaseManager * base, WorkerManager * wm){
	this->buildOrderManager = bom;
	this->productionManager = pm;
	this->buildManager = bm;
	this->baseManager = base;
	this->workerManager = wm;
}

std::string MacroManager::getName() const
{
	return "Macro Manager";
}

void MacroManager::onStart(){

}
void MacroManager::onUnitCreate(BWAPI::Unit* unit)
{
	/*

	// ***********   Expand   *********** //
	if( unit->getType().isResourceDepot())
		expanding = false;

	// *******   ProdBuildings   ******** //
	if( unit->getType().isBuilding() && !unit->getType().isResourceDepot())
	{
		if( unit->getType().canProduce())
		{
			prodBuildings[unit->getType()].push_back( unit);
			underConstruction[unit->getType()]--;
		}
		else if( unit->getType().canAttack())
		{
			defBuildings[unit->getType()].push_back( unit);
			underConstruction[unit->getType()]--;
		}
		else
		{
			techBuildings.insert( unit->getType());
		}
	}

	// ***********   Units   ************ //
	if( !unit->getType().isBuilding() && !unit->getType().isWorker() && unit->getPlayer() == Broodwar->self())
	{
		if (!(unit->getType() == UnitTypes::Zerg_Larva))
		{
			wantedUnits[unit->getType()].completedNb++;
			wantedUnits[unit->getType()].criteria += 1.0 / float(wantedUnits[unit->getType()].plannedValue);
		}
	}
	*/
}

void MacroManager::onUnitDestroy(BWAPI::Unit* unit)
{
	if( !unit->getType().isBuilding() && !unit->getType().isWorker() && unit->getPlayer() == Broodwar->self())
		wantedUnits[unit->getType()].completedNb--;
}

void MacroManager::update()
{
	/*
	buildGeyser();
	trainWorkers();
	createProdBuildings();
	trainUnits();
	createTechBuildings();
	researchTech();
	expand();
	buildDefenses();
	*/
}

void MacroManager::trainWorkers()
{
	/*
	BWAPI::UnitType workerType=(Broodwar->self()->getRace().getWorker());
	if( workerManager->workers.size() >= MAX_WORKERS_NB ||  // max nb worker reached
		workerType.mineralPrice() > buildOrderManager->getUnusedMinerals())
		return;

	// Get remaining places
	const std::set<Base*>& bases = baseManager->getActiveBases();
	unsigned int remainingPlaces;
	if( Broodwar->self()->getRace() != Races::Zerg)
	{
		remainingPlaces = bases.size();
	}
	else
	{
		remainingPlaces = 0;
		const set<Unit*>& myUnits = Broodwar->self()->getUnits();
		for( set<Unit*>::const_iterator itMyUnits = myUnits.begin(); itMyUnits != myUnits.end(); itMyUnits++)
			if( (*itMyUnits)->getType() == UnitTypes::Zerg_Larva)
				remainingPlaces++;
	}

	unsigned int nbMinedBlocks = 0;
	unsigned int nbWorkers = 0;
	for( std::set<Base*>::const_iterator it = bases.begin(); it != bases.end(); it++)
	{
		Unit* ressourceDepot = (*it)->getResourceDepot();
		if( ressourceDepot->isTraining()) continue;
		nbMinedBlocks += (*it)->getMinerals().size();
		// Get nb workers in this base
		const std::set<BWAPI::Unit*>& minBlocks = (*it)->getMinerals();
		for( std::set<BWAPI::Unit*>::const_iterator itBlock = minBlocks.begin(); itBlock != minBlocks.end(); itBlock++)
			nbWorkers += workerManager->currentWorkers[*itBlock].size();
	}

	// if nbWorker <= 2*nbBlock on this base
	if( nbWorkers <= (unsigned int)(2*nbMinedBlocks) &&
		(remainingPlaces - buildOrderManager->getPlannedCount( workerType)) > 0)
		this->buildOrderManager->buildAdditional( 1, workerType, 150);
		*/
}

void MacroManager::trainUnits()
{
	/*
	for( map<UnitType, list<Unit*> >::iterator pb = prodBuildings.begin(); pb != prodBuildings.end(); pb++)
	{
		if( !pb->first.canProduce()) continue;
		int remainingPlaces = remainingTrainingPlace( pb->first); //TODO a passer dans la map des buildings...

		//TODO trier les units selon le ratio
		list<UnitType> sortedUnitsType;
		for( map<UnitType, PlannedUnit>::iterator wu = wantedUnits.begin(); wu != wantedUnits.end(); wu++)
		{
			if(wu->first.whatBuilds().first != pb->first) continue; // Check we are on the good production building.
			list<UnitType>::iterator posToInsert = sortedUnitsType.begin();
			for( list<UnitType>::iterator sut = sortedUnitsType.begin(); sut != sortedUnitsType.end(); sut++)
			{
				if( wu->second.criteria > wantedUnits[*sut].criteria) break;
				posToInsert++;
			}
			sortedUnitsType.insert( posToInsert, wu->first);
		}

		list<UnitType>::iterator st = sortedUnitsType.begin();
		while( remainingPlaces && st != sortedUnitsType.end())
		{
			UnitType type = *st;
			if( canTrainUnits( type, remainingPlaces))
			{
				this->buildOrderManager->buildAdditional( 1, type, 50);
				remainingPlaces--;
				st = sortedUnitsType.begin();
			}
			else
			{
				st++;
			}
		}
	}
	*/
}

void MacroManager::createTechBuildings()
{
}

void MacroManager::researchTech()
{
}

void MacroManager::expand()
{
	/*
	if( expanding ) return;
	if( shouldExpand())
	{
		const std::set<BaseLocation*>& baseLocations = getBaseLocations();
		const std::set<Base*>& activeBases = baseManager->getActiveBases();
		const std::set<Base*>& allBases = baseManager->getAllBases();
		// Search a new expand location.
		double dist = 0xFFFF;
		BaseLocation* nearestLocation = NULL;
		for( std::set<Base*>::const_iterator itBaseFrom = activeBases.begin(); itBaseFrom != activeBases.end(); itBaseFrom++)
		{
			BaseLocation* baseLocFrom = (*itBaseFrom)->getBaseLocation();
			for( std::set<BaseLocation*>::const_iterator itBaseTo = baseLocations.begin(); itBaseTo != baseLocations.end(); itBaseTo++)
			{
				std::map<BWTA::BaseLocation*,Base*>::iterator itUsedBase = baseManager->location2base.find( *itBaseTo);
				if( itUsedBase != baseManager->location2base.end()) continue;
				double dist2 = baseLocFrom->getGroundDistance( *itBaseTo);
				if( dist2 <= 0) continue; //(*it)->minerals() == 0 || // Enable scouting before call this. Minerals are unknown before viewing it
				if( dist2 < dist)
				{
					dist = dist2;
					nearestLocation = *itBaseTo;
				}
			}
		}
		if( nearestLocation)
		{
			this->baseManager->expand( nearestLocation);
			expanding = true;
		}
	}
	*/
}

void MacroManager::buildDefenses()
{
}

void MacroManager::initWantedUnits()
{
}

int MacroManager::remainingTrainingPlace( UnitType type)
{
	unsigned int remainingPlaces = 0;
	const list<Unit*>& buildings = prodBuildings[type];
	for( list<Unit*>::const_iterator it = buildings.begin(); it != buildings.end(); it++)
		if( (*it)->isCompleted() && !(*it)->isTraining())
			remainingPlaces++;
	return remainingPlaces;
}

bool MacroManager::canTrainUnits( UnitType unitType, int remainingPlaces)
{
	bool haveTech = true;
	const std::map<UnitType, int> reqTech = unitType.requiredUnits();
	for( std::map<UnitType, int>::const_iterator it = reqTech.begin(); it != reqTech.end(); it++)
		if(buildManager->getCompletedCount(it->first) < it->second) // what if a building is destroyed?
			haveTech = false;

	return	/*buildOrderManager->getUnusedMinerals() >= unitType.mineralPrice() &&
					buildOrderManager->getUnusedGas() >= unitType.gasPrice() &&*/
					(remainingPlaces - buildOrderManager->getPlannedCount( unitType)) >= 0 &&
					haveTech;
}

bool MacroManager::canCreateTechBuildings( UnitType techBuilding, UnitType buildingRequiered, int nbRequieredBuilding)
{
	return techBuildings.find( techBuilding) == techBuildings.end() &&
		this->buildManager->getPlannedCount( techBuilding) == 0 &&
		(this->buildManager->getCompletedCount( buildingRequiered) >= nbRequieredBuilding ||
		buildingRequiered == UnitTypes::None);
}

bool MacroManager::canCreateDefenseBuildings( UnitType techBuilding, UnitType buildingRequiered)
{
	return prodBuildings[techBuilding].size() < 3 &&
		this->buildManager->getPlannedCount( techBuilding) == 0 &&
		this->buildManager->getCompletedCount( buildingRequiered);
}

bool MacroManager::shouldExpand()
{
	
	// Expand if all the bases are fully functionnal
	unsigned int nbRessources = 0;
	const std::set<Base*>& bases = baseManager->getAllBases();
	for( std::set<Base*>::const_iterator it = bases.begin(); it != bases.end(); it++)
	{
		nbRessources += 2 * (*it)->getMinerals().size() + 3 * (*it)->getGeysers().size();
	}
	return workerManager->workers.size() >= nbRessources;
	
}

#ifdef BW_QT_DEBUG
QWidget* MacroManager::createWidget(QWidget* parent) const
{
	return new QLabel(QString("createWidget and refreshWidget undefined for this component."), parent);
}

void MacroManager::refreshWidget(QWidget* widget) const
{
// TODO update your widget after having defined it in the previous method :)
}
#endif
