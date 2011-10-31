#pragma once
#include <BWAPI.h>
class Resources
{
  public:
    Resources(double m = 0.0, double g = 0.0, double s = 0.0) : minerals(m),gas(g),supply(s) {}
    Resources(BWAPI::Player* player);
    Resources(BWAPI::UnitType type);
    Resources(BWAPI::TechType type);
    Resources(BWAPI::UpgradeType type, int level = 1);
    Resources& operator=(const Resources &r);
    Resources& set(double m, double g = 0, double s = 0);
    Resources& setMinerals(double m);
    Resources& setGas(double g);
    Resources& setSupply(double s);
    Resources& addMinerals(double m);
    Resources& addGas(double g);
    Resources& addSupply(double s);
    Resources& operator+=(const Resources &r);
    Resources& operator-=(const Resources &r);
    Resources& operator*=(double value);
    Resources& operator/=(double value);

    double getMinerals() const;
    double getGas() const;
    double getSupply() const;

    bool operator==(const Resources &r) const;
    bool operator<(const Resources &r) const;
    bool isValid() const;

    Resources operator+(const Resources &r) const;
    Resources operator-(const Resources &r) const;
    Resources operator*(double value) const;
    Resources operator/(double value) const;
    std::string toString() const;

  private:
    double minerals;
    double gas;
    double supply;
};
Resources CumulativeResources(BWAPI::Player* player);