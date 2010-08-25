#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BWTA.h>
#include "CSingleton.h"
#include "Base.h"
#include "UnitsGroup.h"
#include "WarManager.h"
class BorderManager;
class DefenseManager : Arbitrator::Controller<BWAPI::Unit*,double>, public CSingleton<DefenseManager>
{
	friend class CSingleton<DefenseManager>;
public:
  void setDependencies();
  virtual void onOffer(std::set<BWAPI::Unit*> units);
  virtual void onRevoke(BWAPI::Unit* unit, double bid);
  void onRemoveUnit(BWAPI::Unit* unit);
  virtual void update();
  virtual std::string getName() const;
  virtual std::string getShortName() const;

  void checkDefenses();
  void addBase(Base * b); 
private:
	void checkGroundDefense(Base * b, bool toDef);//Check if the right amonts of units is defending 
	//and if the goal is accomplished


  DefenseManager();
  BorderManager* borderManager;
  Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
  WarManager * warManager;

  std::map<Base *, UnitsGroup * > groundDefenders;
  std::map<Base *, UnitsGroup * > airDefenders;
  std::list<UnitsGroup * > requesters;
};