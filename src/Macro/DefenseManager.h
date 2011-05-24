#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BWTA.h>
class BorderManager;
class DefenseManager : Arbitrator::Controller<BWAPI::Unit*,double>
{
public:
  class DefenseData
  {
    public:
      enum DefenseMode
      {
        Idle,
        Moving
      };
      DefenseData(){ mode = Idle; }
      DefenseMode mode;
  };
  DefenseManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator);
  void setBorderManager(BorderManager* borderManager);
  virtual void onOffer(std::set<BWAPI::Unit*> units);
  virtual void onRevoke(BWAPI::Unit* unit, double bid);
  void onRemoveUnit(BWAPI::Unit* unit);
  virtual void update();
  virtual std::string getName() const;
  virtual std::string getShortName() const;

private:
  BorderManager* borderManager;
  Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
  std::set<BWTA::Chokepoint*> myBorder;
  std::vector<BWTA::Chokepoint*> myBorderVector;
  std::map<BWAPI::Unit*,DefenseData> defenders;
};