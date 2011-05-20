#include <RectangleArray.h>
#include <map>
#include <vector>
void log(const char* text, ...);
std::map<int, int> computeAssignments(Util::RectangleArray< double> &cost);
void myRestartGame();
void logScore(bool isWinner, std::string mapName);
std::vector<double> exponentialDistribution(double lambda, unsigned int bins=1000);