#include <DefenseManager.h>
#include <BorderManager.h>
DefenseManager::DefenseManager()
{
  this->arbitrator = NULL;
}
void DefenseManager::setDependencies(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arb, BorderManager* borderManager)
{
  this->arbitrator= arb;
  this->borderManager=borderManager;
}
void DefenseManager::onOffer(std::set<BWAPI::Unit*> units)
{
	/*
  for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
  {
    if (defenders.find(*u) == defenders.end())
    {
      arbitrator->accept(this, *u);
      DefenseData temp;
      defenders.insert(std::make_pair(*u, temp));
    }
  }
  */
}

void DefenseManager::onRevoke(BWAPI::Unit* unit, double bid)
{
 // defenders.erase(unit);
}

void DefenseManager::onRemoveUnit(BWAPI::Unit* unit)
{
//  defenders.erase(unit);
}

void DefenseManager::update()
{

}

std::string DefenseManager::getName() const
{
  return "Defense Manager";
}

std::string DefenseManager::getShortName() const
{
  return "Def";
}