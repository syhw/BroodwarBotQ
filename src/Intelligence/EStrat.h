#pragma once
#include "EEcoEstimator.h"

class EStrat : public CSingleton<EStrat>
{
	friend class CSingleton<EStrat>;

public :
	double probability_of_rush();
	double probability_of_proxy();
	double probability_of_canonrush();
};