#include <PrecompiledHeader.h>
#include "Macro/Resources.h"
Resources::Resources(BWAPI::Player* player)
{
	if (player==NULL)
		player = BWAPI::Broodwar->self();
	minerals = player->minerals();
	gas      = player->gas();
	supply   = player->supplyTotal() - player->supplyUsed();
}
Resources CumulativeResources(BWAPI::Player* player)
{
	if (player==NULL)
		player = BWAPI::Broodwar->self();
	return Resources(player->gatheredMinerals(),player->gatheredGas(),player->supplyTotal() - player->supplyUsed());
}
Resources::Resources(BWAPI::UnitType type)
{
	minerals = type.mineralPrice();
	gas      = type.gasPrice();
	supply   = type.supplyRequired();
}
Resources::Resources(BWAPI::TechType type)
{
	minerals = type.mineralPrice();
	gas      = type.gasPrice();
	supply   = 0.0;
}
Resources::Resources(BWAPI::UpgradeType type, int level)
{
	minerals = type.mineralPrice() + type.mineralPriceFactor()*(level-1);
	gas      = type.gasPrice() + type.gasPriceFactor()*(level-1);
	supply   = 0.0;
}
Resources& Resources::operator=(const Resources &r)
{
	minerals = r.minerals;
	gas      = r.gas;
	supply   = r.supply;
	return *this;
}
Resources& Resources::set(double m, double g, double s)
{
	minerals = m;
	gas      = g;
	supply   = s;
	return *this;
}
Resources& Resources::setMinerals(double m)
{
	minerals = m;
	return *this;
}
Resources& Resources::setGas(double g)
{
	gas = g;
	return *this;
}
Resources& Resources::setSupply(double s)
{
	supply = s;
	return *this;
}
Resources& Resources::addMinerals(double m)
{
	minerals += m;
	return *this;
}
Resources& Resources::addGas(double g)
{
	gas += g;
	return *this;
}
Resources& Resources::addSupply(double s)
{
	supply += s;
	return *this;
}
Resources& Resources::operator+=(const Resources &r)
{
	minerals += r.minerals;
	gas      += r.gas;
	supply   += r.supply;
	return *this;
}
Resources& Resources::operator-=(const Resources &r)
{
	minerals -= r.minerals;
	gas      -= r.gas;
	supply   -= r.supply;
	return *this;
}
Resources& Resources::operator*=(double value)
{
	minerals *= value;
	gas      *= value;
	supply   *= value;
	return *this;
}
Resources& Resources::operator/=(double value)
{
	minerals /= value;
	gas      /= value;
	supply   /= value;
	return *this;
}
double Resources::getMinerals() const
{
	return minerals;
}
double Resources::getGas() const
{
	return gas;
}
double Resources::getSupply() const
{
	return supply;
}
bool Resources::operator==(const Resources &r) const
{
	return minerals == r.minerals && gas == r.gas && supply == r.supply;
}
bool Resources::operator<(const Resources &r) const
{
	return minerals < r.minerals || (minerals == r.minerals && gas < r.gas) || (minerals == r.minerals && gas == r.gas && supply < r.supply);
}
bool Resources::isValid() const
{
	return minerals>=0 && gas>=0 && supply>=0;
}
Resources Resources::operator+(const Resources &r) const
{
	return Resources(minerals+r.minerals,gas+r.gas,supply+r.supply);
}
Resources Resources::operator-(const Resources &r) const
{
	return Resources(minerals-r.minerals,gas-r.gas,supply-r.supply);
}
Resources Resources::operator*(double value) const
{
	return Resources(minerals*value,gas*value,supply*value);
}
Resources Resources::operator/(double value) const
{
	return Resources(minerals/value,gas/value,supply/value);
}
std::string Resources::toString() const
{
	std::stringstream s;
	s << "(Minerals = " << minerals << ", Gas = " << gas << ", Supply = " << supply << ")";
	return s.str();
}
