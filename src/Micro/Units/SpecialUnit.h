#pragma once

#include "BayesianUnit.h"

class SpecialUnit : public BayesianUnit
{
    public:
        SpecialUnit(BWAPI::Unit* u, UnitsGroup* ug);
        ~SpecialUnit();
        virtual void micro() = 0;
        virtual bool canHit(BWAPI::Unit* enemy) = 0;
        virtual int damagesOn(BWAPI::Unit* enemy);
        virtual int getTimeToAttack() = 0;
};