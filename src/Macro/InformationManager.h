#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <map>
class InformationManager
{
  public:
    static InformationManager* create();
    static void destroy();
    void onUnitDiscover(BWAPI::Unit* unit);
    void onUnitEvade(BWAPI::Unit* unit);
    void onUnitDestroy(BWAPI::Unit* unit);
    BWAPI::Player* getPlayer(BWAPI::Unit* unit) const;
    BWAPI::UnitType getType(BWAPI::Unit* unit) const;
    BWAPI::Position getLastPosition(BWAPI::Unit* unit) const;
    int getLastSeenTime(BWAPI::Unit* unit) const;
    bool exists(BWAPI::Unit* unit) const;
    bool enemyHasBuilt(BWAPI::UnitType type) const;
    int getBuildTime(BWAPI::UnitType type) const;
    const std::set<BWTA::BaseLocation*>& getEnemyBases() const;
    void setBaseEmpty(BWTA::BaseLocation* base);
  private:
    InformationManager();
    ~InformationManager();
    class UnitData
    {
      public:
        UnitData();
        BWAPI::Position position;
        BWAPI::UnitType type;
        BWAPI::Player* player;
        int lastSeenTime;
        bool exists;
    };
    void updateBuildTime(BWAPI::UnitType type, int time);
    std::map<BWAPI::Unit*, UnitData> savedData;
    std::map<BWAPI::UnitType, int> buildTime;
    std::set<BWTA::BaseLocation*> enemyBases;
    std::map<BWTA::BaseLocation*,BWAPI::Unit*> enemyBaseCenters;
    std::set<BWTA::BaseLocation*> startLocationCouldContainEnemy;
    bool scoutedAnEnemyBase;
};
extern InformationManager* TheInformationManager;