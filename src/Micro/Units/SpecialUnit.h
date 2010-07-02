#pragma once

#include "BayesianUnit.h"

class SpecialUnit : public BayesianUnit
{
    public:
        SpecialUnit(BWAPI::Unit* u, UnitsGroup* ug);
        ~SpecialUnit();
        virtual void micro() = 0;
};