#pragma once

#include "BayesianUnit.h"

class FlyingUnit : public BayesianUnit
{
    public:
        FlyingUnit(BWAPI::Unit* u, UnitsGroup* ug);
        ~FlyingUnit();
        virtual void micro() = 0;
};