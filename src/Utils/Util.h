#pragma once
#include <Macro/RectangleArray.h>
#include <map>
#include <vector>
#include <set>
void log(const char* text, ...);
std::map<int, int> computeAssignments(Util::RectangleArray< double> &cost);
void myRestartGame();
void logScore(bool isWinner, std::string mapName);
std::vector<double> exponentialDistribution(double lambda, unsigned int bins=1000);
size_t indMax(const std::vector<long double>& t);
std::set<size_t> supTo(const std::vector<long double>& t, long double minProb);