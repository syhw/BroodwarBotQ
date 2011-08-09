#include <PrecompiledHeader.h>
#include "Macro/BorderManager.h"
#include "Macro/InformationManager.h"
#include "Defines.h"

BorderManager* TheBorderManager = NULL;

BorderManager* BorderManager::create()
{
	if (TheBorderManager) return TheBorderManager;
	return new BorderManager();
}
void BorderManager::destroy()
{
	if (TheBorderManager)
		delete TheBorderManager;
}
BorderManager::BorderManager()
{
	TheBorderManager = this;
}
BorderManager::~BorderManager()
{
	TheBorderManager = NULL;
}
void BorderManager::addMyBase(BWTA::BaseLocation* location)
{
	this->myBases.insert(location);
	recalculateBorders();
}
void BorderManager::removeMyBase(BWTA::BaseLocation* location)
{
	this->myBases.erase(location);
	recalculateBorders();
}
const std::set<BWTA::Chokepoint*>& BorderManager::getMyBorder() const
{
	return this->myBorder;
}
const std::set<BWTA::Region*>& BorderManager::getMyRegions() const
{
	return this->myRegions;
}
const std::set<BWTA::Chokepoint*>& BorderManager::getEnemyBorder() const
{
	return this->enemyBorder;
}
void BorderManager::update()
{
	if (TheInformationManager->getEnemyBases()!=this->enemyBases)
	{
		this->enemyBases=TheInformationManager->getEnemyBases();
		recalculateBorders();
	}
	for(std::set<BWTA::Chokepoint*>::const_iterator c=myBorder.begin();c!=myBorder.end();c++)
	{
		BWAPI::Position point1=(*c)->getSides().first;
		BWAPI::Position point2=(*c)->getSides().second;
#ifdef __DEBUG__
		BWAPI::Broodwar->drawLineMap(point1.x(),point1.y(),point2.x(),point2.y(),BWAPI::Colors::Red);
#endif
	}
	for(std::set<BWTA::Chokepoint*>::const_iterator c=enemyBorder.begin();c!=enemyBorder.end();c++)
	{
		BWAPI::Position point1=(*c)->getSides().first;
		BWAPI::Position point2=(*c)->getSides().second;
#ifdef __DEBUG__
		BWAPI::Broodwar->drawLineMap(point1.x(),point1.y(),point2.x(),point2.y(),BWAPI::Colors::Orange);
#endif
	}
}

void BorderManager::recalculateBorders()
{
	this->myRegions.clear();
	this->myBorder.clear();
	this->enemyRegions.clear();
	this->enemyBorder.clear();
	std::set<BWTA::Region*> canReachSelf;
	std::set<BWTA::Region*> canReachEnemy;
	for(std::set<BWTA::BaseLocation*>::iterator i=myBases.begin();i!=myBases.end();i++)
	{
		myRegions.insert((*i)->getRegion());
		canReachSelf.insert((*i)->getRegion());
	}
	for(std::set<BWTA::BaseLocation*>::iterator i=enemyBases.begin();i!=enemyBases.end();i++)
	{
		enemyRegions.insert((*i)->getRegion());
		canReachEnemy.insert((*i)->getRegion());
	}
	if (!enemyBases.empty())
	{
		bool exploring=true;
		while (exploring)
		{
			exploring=false;
			for(std::set<BWTA::Region*>::const_iterator i=BWTA::getRegions().begin();i!=BWTA::getRegions().end();i++)
			{
				BWTA::Region* r=*i;
				for(std::set<BWTA::Chokepoint*>::const_iterator c=r->getChokepoints().begin();c!=r->getChokepoints().end();c++)
				{
					BWTA::Region* r2=(*c)->getRegions().first;
					if (r==r2)
						r2=(*c)->getRegions().second;

					if (canReachSelf.find(r)!=canReachSelf.end() && enemyRegions.find(r2)==enemyRegions.end() && canReachSelf.find(r2)==canReachSelf.end())
					{
						canReachSelf.insert(r2);
						exploring=true;
					}
					if (canReachEnemy.find(r)!=canReachEnemy.end() && myRegions.find(r2)==myRegions.end() && canReachEnemy.find(r2)==canReachEnemy.end())
					{
						canReachEnemy.insert(r2);
						exploring=true;
					}
				}
			}
		}
	}
	for(std::set<BWTA::Region*>::const_iterator i=BWTA::getRegions().begin();i!=BWTA::getRegions().end();i++)
	{
		BWTA::Region* r=*i;
		if (canReachSelf.find(r)!=canReachSelf.end() && canReachEnemy.find(r)==canReachEnemy.end())
			myRegions.insert(r);
		if (canReachSelf.find(r)==canReachSelf.end() && canReachEnemy.find(r)!=canReachEnemy.end())
			enemyRegions.insert(r);
	}
	for(std::set<BWTA::Region*>::iterator i=myRegions.begin();i!=myRegions.end();i++)
	{
		BWTA::Region* r=*i;
		for(std::set<BWTA::Chokepoint*>::const_iterator c=r->getChokepoints().begin();c!=r->getChokepoints().end();c++)
		{
			if (myBorder.find(*c)==myBorder.end())
				myBorder.insert(*c);
			else
				myBorder.erase(*c);
		}
	}
	for(std::set<BWTA::Region*>::iterator i=enemyRegions.begin();i!=enemyRegions.end();i++)
	{
		BWTA::Region* r=*i;
		for(std::set<BWTA::Chokepoint*>::const_iterator c=r->getChokepoints().begin();c!=r->getChokepoints().end();c++)
		{
			if (enemyBorder.find(*c)==enemyBorder.end())
				enemyBorder.insert(*c);
			else
				enemyBorder.erase(*c);
		}
	}
}