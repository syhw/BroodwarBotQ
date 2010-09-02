#pragma once

#include "BayesianUnit.h"

class GroundUnit : public BayesianUnit
{
    public:
        GroundUnit(BWAPI::Unit* u, UnitsGroup* ug);
        ~GroundUnit();
        virtual void micro() = 0;
        virtual void check() = 0;
        virtual bool canHit(BWAPI::Unit* enemy) = 0;
        virtual int damagesOn(BWAPI::Unit* enemy);
        virtual int getTimeToAttack() = 0;
        virtual bool withinRange(BWAPI::Unit* enemy);
        virtual BWAPI::UnitType* getListPriorite() = 0;
};