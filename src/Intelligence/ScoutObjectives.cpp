#include "ScoutObjectives.h"
#include <iostream>
using namespace BWAPI;


ScoutObjectives::ScoutObjectives()
{

}

ScoutObjectives::~ScoutObjectives(){

}

bool ScoutObjectives::ennemy_found() const{
 return bEnnemy_found;
}

void ScoutObjectives::set_ennemy_found(bool b){
	bEnnemy_found=b;
}

std::list<Position> ScoutObjectives::get_Objectives(){
std::list<Position> temp;
return temp;
}

bool ScoutObjectives::got_objectives(){

	return objectives.size() > 0;
}




void ScoutObjectives::accomplished(BWAPI::Position p){


this->assigned_objectives.remove(p);
}

void ScoutObjectives::add(Position p){
	to_display.push_back(p);
}

void ScoutObjectives::show(){
	for each (Position pos in to_display){
		Broodwar->drawCircleMap(pos.x(),pos.y(),20,Colors::Blue);
	}
}