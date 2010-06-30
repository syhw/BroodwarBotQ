#pragma once

#include "BayesianUnit.h"

class GroundUnit : public BayesianUnit
{
    public:
        GroundUnit(BWAPI::Unit* u, UnitsGroup* ug);
        ~GroundUnit();
        virtual void micro() = 0;
};