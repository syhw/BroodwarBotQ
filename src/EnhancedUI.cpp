#include <EnhancedUI.h>
using namespace BWAPI;

void EnhancedUI::update() const
{
  drawTerrain();
  drawBases();
  drawProgress();
}

void EnhancedUI::drawBases() const
{
  //we will iterate through all the base locations, and draw their outlines.
  for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
  {
    TilePosition p=(*i)->getTilePosition();
    Position c=(*i)->getPosition();

    //draw outline of center location
    Broodwar->drawBox(CoordinateType::Map,p.x()*32,p.y()*32,p.x()*32+4*32,p.y()*32+3*32,Colors::Blue,false);

    //draw a circle at each mineral patch
    for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getMinerals().begin();j!=(*i)->getMinerals().end();j++)
    {
      Position q=(*j)->getPosition();
      Broodwar->drawCircle(CoordinateType::Map,q.x(),q.y(),30,Colors::Cyan,false);
    }

    //draw the outlines of vespene geysers
    for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getGeysers().begin();j!=(*i)->getGeysers().end();j++)
    {
      TilePosition q=(*j)->getTilePosition();
      Broodwar->drawBox(CoordinateType::Map,q.x()*32,q.y()*32,q.x()*32+4*32,q.y()*32+2*32,Colors::Orange,false);
    }

    //if this is an island expansion, draw a yellow circle around the base location
    if ((*i)->isIsland())
    {
      Broodwar->drawCircle(CoordinateType::Map,c.x(),c.y(),80,Colors::Yellow,false);
    }
  }
}

void EnhancedUI::drawTerrain() const
{
  //we will iterate through all the regions and draw the polygon outline of it in green.
  for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
  {
    BWTA::Polygon p=(*r)->getPolygon();
    for(int j=0;j<(int)p.size();j++)
    {
      Position point1=p[j];
      Position point2=p[(j+1) % p.size()];
      Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Green);
    }
  }

  //we will visualize the chokepoints with yellow lines
  for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
  {
    for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();c++)
    {
      Position point1=(*c)->getSides().first;
      Position point2=(*c)->getSides().second;
      Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Yellow);
    }
  }
}

void EnhancedUI::drawProgress() const
{
  UnitGroup constructing = SelectAll()(isBuilding).not(isCompleted);
  for each (Unit* c in constructing)
  {
    double progress = 1.0 - (static_cast<double>(c->getRemainingBuildTime()) / c->getType().buildTime());
    drawProgressBar(c->getPosition(), progress, BWAPI::Colors::Red);
  }

  UnitGroup producing = SelectAll()(isTraining);
  for each (Unit* c in producing)
  {
    if (c->getRemainingTrainTime() > .0)
    {
      double progress = 1.0 - (static_cast<double>(c->getRemainingTrainTime()) / c->getTrainingQueue().front().buildTime());
      drawProgressBar(c->getPosition(), progress, BWAPI::Colors::Green);
    }
  }

}

void EnhancedUI::drawProgressBar(BWAPI::Position pos, double progressFraction, BWAPI::Color innerBar) const
{
  const int width = 20, height = 4;
  const BWAPI::Color outline = BWAPI::Colors::Blue;
  const BWAPI::Color barBG = BWAPI::Color(0, 0, 170);
  int xLeft = pos.x() - width / 2, xRight = pos.x() + width / 2;
  int yTop = pos.y() - height / 2, yBottom = pos.y() + height / 2;
  
  //Draw outline
  Broodwar->drawLineMap(xLeft + 1, yTop, xRight - 1, yTop, outline);        //top
  Broodwar->drawLineMap(xLeft + 1, yBottom, xRight - 1, yBottom, outline);  //bottom
  Broodwar->drawLineMap(xLeft, yTop + 1, xLeft, yBottom, outline);          //left
  Broodwar->drawLineMap(xRight - 1, yTop + 1, xRight - 1, yBottom, outline);//right
  //Draw bar
  Broodwar->drawBoxMap(xLeft + 1, yTop + 1, xRight - 1, yBottom, barBG, true);
  //Draw progress bar
  const int innerWidth = (xRight - 1) - (xLeft + 1);
  int progressWidth = static_cast<int>(progressFraction * innerWidth);
  Broodwar->drawBoxMap(xLeft + 1, yTop + 1, (xLeft + 1) + progressWidth, yBottom, innerBar, true);
}