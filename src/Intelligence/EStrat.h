#pragma once
#include "EEcoEstimator"

class EStrat : public CSingleton<EStrat>
{
	friend class CSingleton<EEcoEstimator>;

public :
	int probability_of_rush();
	
}