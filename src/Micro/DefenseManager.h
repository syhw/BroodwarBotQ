#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BWTA.h>
#include "CSingleton.h"
#include "Base.h"
#include "UnitsGroup.h"
class BorderManager;
class DefenseManager : Arbitrator::Controller<BWAPI::Unit*,double>, public CSingleton<DefenseManager>
{
	friend class CSingleton<DefenseManager>;
public:
  void setDependencies(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arb, BorderManager* borderManager);
  virtual void onOffer(std::set<BWAPI::Unit*> units);
  virtual void onRevoke(BWAPI::Unit* unit, double bid);
  void onRemoveUnit(BWAPI::Unit* unit);
  virtual void update();
  virtual std::string getName() const;
  virtual std::string getShortName() const;

private:
  DefenseManager();
  BorderManager* borderManager;
  Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
  std::map<Base *,std::set<UnitsGroup *> > defenders;
};