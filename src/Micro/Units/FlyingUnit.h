#pragma once

#include "BayesianUnit.h"

class FlyingUnit : public BayesianUnit
{
    public:
        FlyingUnit(BWAPI::Unit* u, UnitsGroup* ug);
        ~FlyingUnit();
        virtual void micro() = 0;
        virtual bool canHit(BWAPI::Unit* enemy) = 0;
        virtual int damagesOn(BWAPI::Unit* enemy);
        virtual int getTimeToAttack() = 0;
};