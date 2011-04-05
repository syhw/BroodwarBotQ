#include <PrecompiledHeader.h>
#include "EStrat.h"

double EStrat::probability_of_rush(){
	//To be computed on scout operation
	
	
	//TODO determine whether rush is only showed by 
	//the number of 'missing' worker
	//compared to the theorical number
	return 0.5;
}

double EStrat::probability_of_proxy(){
	//We need to know if a worker is missing
	return 0.5;
}

double EStrat::probability_of_canonrush(){
	//We need to know if a worker is missing
	//If a forge was created
	return 0.5;
}