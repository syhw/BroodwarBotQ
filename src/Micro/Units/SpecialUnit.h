#pragma once

#include "BayesianUnit.h"

class SpecialUnit : public BayesianUnit
{
    public:
        SpecialUnit(BWAPI::Unit* u, UnitsGroup* ug);
        ~SpecialUnit();
        virtual void micro() = 0;
        virtual void check() = 0;
        virtual int getAttackDuration();
        virtual std::set<BWAPI::UnitType> getSetPrio() = 0;
};