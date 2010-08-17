#pragma once
#include <BWAPI.h>
#include <BayesianUnit.h>
class EUnit
{
    private:
        BWAPI::Unit* m_self;
        int m_damageTaken;
        int m_hp;
        double m_distance;
    public:
        EUnit(BWAPI::Unit* self, double distance);
        double& distance();
        void update();
        int& damageTaken();
        BWAPI::Unit* self();
        int getPrio(pBayesianUnit u);
};