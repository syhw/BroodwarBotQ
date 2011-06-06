#include <PrecompiledHeader.h>
#include "Macro/MacroBaseManager.h"
#include "Macro/BorderManager.h"
using namespace BWAPI;

MacroBaseManager* TheMacroBaseManager = NULL;
MacroBaseManager* MacroBaseManager::create()
{
  if (TheMacroBaseManager) return TheMacroBaseManager;
  return new MacroBaseManager();
}
void MacroBaseManager::destroy()
{
  if (TheMacroBaseManager)
    delete TheMacroBaseManager;
}
MacroBaseManager::MacroBaseManager()
{
  TheMacroBaseManager=this;
}
MacroBaseManager::~MacroBaseManager()
{
  TheMacroBaseManager = NULL;
}
void MacroBaseManager::update()
{
  //check to see if any new base locations need to be added
  for each(BWTA::BaseLocation* location in BWTA::getBaseLocations())
  {
    if (location2base.find(location) == location2base.end())
    {
      TilePosition tile = location->getTilePosition();
      for each(Unit* u in Broodwar->getUnitsOnTile(tile.x(), tile.y()))
        if (u->getPlayer() == Broodwar->self() && u->getType().isResourceDepot())
        {
          MacroBase* mb = MacroBase::RecognizeExistingBase(location,u);
          location2base[location] = mb;
          allBases.insert(mb);
          TheBorderManager->addMyBase(location);
        }
    }
  }
  for each(MacroBase* mb in allBases)
  {
    mb->update();
    if (mb->isActive())
      activeBases.insert(mb);
    else
      activeBases.erase(mb);
    if (mb->isReady())
      readyBases.insert(mb);
    else
      readyBases.erase(mb);
  }
}

MacroBase* MacroBaseManager::getBase(BWTA::BaseLocation* location)
{
  std::map<BWTA::BaseLocation*,MacroBase*>::iterator i=location2base.find(location);
  if (i==location2base.end())
    return NULL;
  return i->second;
}
MacroBase* MacroBaseManager::expandNow(BWTA::BaseLocation* location, bool getGas)
{
  if (location == NULL)
  {
    BWTA::BaseLocation* home=BWTA::getStartLocation(BWAPI::Broodwar->self());
    double minDist = -1;
    for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
    {
      double dist=home->getGroundDistance(*i);
      if (dist>0 && getBase(*i)==NULL)
      {
        if (minDist == -1 || dist<minDist)
        {
          minDist=dist;
          location=*i;
        }
      }
    }
  }
  if (location==NULL) return NULL;
  MacroBase* mb = MacroBase::CreateBaseNow(location,getGas);
  location2base[location] = mb;
  allBases.insert(mb);
  TheBorderManager->addMyBase(location);
 return mb;
}
MacroBase* MacroBaseManager::expandWhenPossible(BWTA::BaseLocation* location, bool getGas)
{
  if (location == NULL)
  {
    BWTA::BaseLocation* home=BWTA::getStartLocation(BWAPI::Broodwar->self());
    double minDist = -1;
    for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
    {
      double dist=home->getGroundDistance(*i);
      if (dist>0 && getBase(*i)==NULL)
      {
        if (minDist == -1 || dist<minDist)
        {
          minDist=dist;
          location=*i;
        }
      }
    }
  }
  if (location==NULL) return NULL;
  MacroBase* mb = MacroBase::CreateBaseWhenPossible(location,getGas);
  location2base[location] = mb;
  allBases.insert(mb);
  TheBorderManager->addMyBase(location);
  return mb;
}
MacroBase* MacroBaseManager::expandAtFrame(int frame, BWTA::BaseLocation* location, bool getGas)
{
  if (location == NULL)
  {
    BWTA::BaseLocation* home=BWTA::getStartLocation(BWAPI::Broodwar->self());
    double minDist = -1;
    for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
    {
      double dist=home->getGroundDistance(*i);
      if (dist>0 && getBase(*i)==NULL)
      {
        if (minDist == -1 || dist<minDist)
        {
          minDist=dist;
          location=*i;
        }
      }
    }
  }
  if (location==NULL) return NULL;
  MacroBase* mb = MacroBase::CreateBaseAtFrame(location,frame,getGas);
  location2base[location] = mb;
  allBases.insert(mb);
  TheBorderManager->addMyBase(location);
  return mb;
}
const std::set<MacroBase*>& MacroBaseManager::getActiveBases() const
{
  return activeBases;
}
const std::set<MacroBase*>& MacroBaseManager::getReadyBases() const
{
  return readyBases;
}
const std::set<MacroBase*>& MacroBaseManager::getAllBases() const
{
  return allBases;
}
const std::set<MacroBase*>& MacroBaseManager::getDestroyedBases() const
{
  return destroyedBases;
}

std::string MacroBaseManager::getName()
{
  return "MacroBaseManager";
}
void MacroBaseManager::onUnitDestroy(BWAPI::Unit* unit)
{
  for each(MacroBase* b in allBases)
  {
    b->onUnitDestroy(unit);
  }
}

