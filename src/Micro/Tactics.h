#pragma once
#include <vector>
#include <set>
#include <BWTA.h>
#include "Micro/Tactics.h"

#define AD_CARDINALITY 4
#define GD_CARDINALITY 4
#define ID_CARDINALITY 3
#define H_CARDINALITY 4

#define E_CARDINALITY 3
#define T_CARDINALITY 6
#define TA_CARDINALITY 6
#define B_CARDINALITY 2
#define A_CARDINALITY 2

struct AD_GD_ID_knowing_H 
{
	std::vector<std::vector<std::vector<std::vector<double> > > > t;
	AD_GD_ID_knowing_H()
		: t(std::vector<std::vector<std::vector<std::vector<double> > > >(AD_CARDINALITY, 
		std::vector<std::vector<std::vector<double> > >(GD_CARDINALITY,
		std::vector<std::vector<double> >(ID_CARDINALITY, 
		std::vector<double>(H_CARDINALITY, 1.0/(AD_CARDINALITY*GD_CARDINALITY*ID_CARDINALITY))))))
	{
	}
	double& operator()(const int ad_i, const int gd_i, const int id_i, const int h_i)
	{
		assert(ad_i >= 0 && ad_i < AD_CARDINALITY);
		assert(gd_i >= 0 && gd_i < GD_CARDINALITY);
		assert(id_i >= 0 && id_i < ID_CARDINALITY);
		assert(h_i >= 0 && h_i < H_CARDINALITY);
		return t[ad_i][gd_i][id_i][h_i];
	}
};

struct E_T_TA_B_knowing_A
{
	std::vector<std::vector<std::vector<std::vector<std::vector<double> > > > > t;
	E_T_TA_B_knowing_A()
		: t(std::vector<std::vector<std::vector<std::vector<std::vector<double> > > > >(E_CARDINALITY, 
		std::vector<std::vector<std::vector<std::vector<double> > > >(T_CARDINALITY,
		std::vector<std::vector<std::vector<double> > >(TA_CARDINALITY,
		std::vector<std::vector<double> >(B_CARDINALITY, 
		std::vector<double>(A_CARDINALITY, 1.0/(E_CARDINALITY*T_CARDINALITY*TA_CARDINALITY*B_CARDINALITY)))))))
	{
	}
	double& operator()(const int e_i, const int t_i, const int ta_i, const int b_i, const int a_i)
	{
		assert(e_i >= 0 && e_i < E_CARDINALITY);
		assert(t_i >= 0 && t_i < T_CARDINALITY);
		assert(ta_i >= 0 && ta_i < TA_CARDINALITY);
		assert(b_i >= 0 && a_i < B_CARDINALITY);
		assert(a_i >= 0 && a_i < A_CARDINALITY);
		return t[e_i][t_i][ta_i][b_i][a_i];
	}
};

struct H_knowing_HP
{
	std::vector<std::vector<double> > t;
	H_knowing_HP()
		: t(std::vector<std::vector<double> >(H_CARDINALITY,
		std::vector<double>(8, 1.0/H_CARDINALITY)))
	{
	}
	double& operator()(const int h_i, const int hp_i)
	{
		assert(h_i >= 0 && h_i < H_CARDINALITY);
		assert(hp_i >= 0 && hp_i < 8);
		return t[h_i][hp_i];
	}
};

class Tactics 
{
	E_T_TA_B_knowing_A e_t_ta_b_a;
	AD_GD_ID_knowing_H ad_gd_id_h;
	H_knowing_HP h_hp;
	std::vector<double> HP_from_TT;
	std::map<BWTA::Region*, double> attack_regions; // attack_regions[reg]
	std::map<BWTA::Region*, std::vector<double> > how_regions; // how_regions[reg][how]

	std::map<BWTA::Region*, std::vector<double> > getBelongs(const std::set<BWTA::Region*>& d_regs, const std::set<BWTA::Region*>& a_regs);
	std::map<BWTA::Region*, std::vector<double> > getTacticals(BWAPI::Player* p);
	std::vector<double> getEconomy(BWTA::Region* r, BWAPI::Player* p);
	std::vector<double> getInvisibleDef(BWTA::Region* r, BWAPI::Player* p);
	std::vector<double> getGroundDef(BWTA::Region* r, BWAPI::Player* d, BWAPI::Player* a);
	std::vector<double> getAirDef(BWTA::Region* r, BWAPI::Player* d, BWAPI::Player* a);
public:
	void predictTheirAttack();
	double getAttackRegion(BWTA::Region* r);
	const std::vector<double>& getHowRegion(BWTA::Region* r);
};