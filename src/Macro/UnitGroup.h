#pragma once
#include <BWAPI.h>
#include <BWTA.h>
enum FilterFlag
{
  exists = 1,
  isAccelerating,
  isBeingConstructed,
  isBeingGathered,
  isBeingHealed,
  isBlind,
  isBraking,
  isBurrowed,
  isCarryingGas,
  isCarryingMinerals,
  isCloaked,
  isCompleted,
  isConstructing,
  isDefenseMatrixed,
  isEnsnared,
  isFollowing,
  isGatheringGas,
  isGatheringMinerals,
  isHallucination,
  isIdle,
  isIrradiated,
  isLifted,
  isLoaded,
  isLockedDown,
  isMaelstrommed,
  isMorphing,
  isMoving,
  isParasited,
  isPatrolling,
  isPlagued,
  isRepairing,
  isResearching,
  isSelected,
  isSieged,
  isStartingAttack,
  isStasised,
  isStimmed,
  isTraining,
  isUnderStorm,
  isUnpowered,
  isUpgrading,
  isVisible,
  canProduce,
  canAttack,
  canMove,
  isFlyer,
  regeneratesHP,
  isSpellcaster,
  hasPermanentCloak,
  isInvincible,
  isOrganic,
  isMechanical,
  isRobotic,
  isDetector,
  isResourceContainer,
  isResourceDepot,
  isRefinery,
  isWorker,
  requiresPsi,
  requiresCreep,
  isTwoUnitsInOneEgg,
  isBurrowable,
  isCloakable,
  isBuilding,
  isAddon,
  isFlyingBuilding,
  isNeutral,
  Marine,
  Ghost,
  Vulture,
  Goliath,
  Siege_Tank,
  SCV,
  Wraith,
  Science_Vessel,
  Dropship,
  Battlecruiser,
  Vulture_Spider_Mine,
  Nuclear_Missile,
  Firebat,
  Scanner_Sweep,
  Medic,
  Larva,
  Egg,
  Zergling,
  Hydralisk,
  Ultralisk,
  Broodling,
  Drone,
  Overlord,
  Mutalisk,
  Guardian,
  Queen,
  Defiler,
  Scourge,
  Infested_Terran,
  Valkyrie,
  Cocoon,
  Corsair,
  Dark_Templar,
  Devourer,
  Dark_Archon,
  Probe,
  Zealot,
  Dragoon,
  High_Templar,
  Archon,
  Shuttle,
  Scout,
  Arbiter,
  Carrier,
  Interceptor,
  Reaver,
  Observer,
  Scarab,
  Rhynadon,
  Bengalaas,
  Scantid,
  Kakaru,
  Ragnasaur,
  Ursadon,
  Lurker_Egg,
  Lurker,
  Disruption_Web,
  Command_Center,
  Comsat_Station,
  Nuclear_Silo,
  Supply_Depot,
  Refinery,
  Barracks,
  Academy,
  Factory,
  Starport,
  Control_Tower,
  Science_Facility,
  Covert_Ops,
  Physics_Lab,
  Machine_Shop,
  Engineering_Bay,
  Armory,
  Missile_Turret,
  Bunker,
  Crashed_Norad_II,
  Ion_Cannon,
  Infested_Command_Center,
  Hatchery,
  Lair,
  Hive,
  Nydus_Canal,
  Hydralisk_Den,
  Defiler_Mound,
  Greater_Spire,
  Queens_Nest,
  Evolution_Chamber,
  Ultralisk_Cavern,
  Spire,
  Spawning_Pool,
  Creep_Colony,
  Spore_Colony,
  Sunken_Colony,
  Overmind_With_Shell,
  Overmind,
  Extractor,
  Mature_Chrysalis,
  Cerebrate,
  Cerebrate_Daggoth,
  Nexus,
  Robotics_Facility,
  Pylon,
  Assimilator,
  Observatory,
  Gateway,
  Photon_Cannon,
  Citadel_of_Adun,
  Cybernetics_Core,
  Templar_Archives,
  Forge,
  Stargate,
  Stasis_Cell_Prison,
  Fleet_Beacon,
  Arbiter_Tribunal,
  Robotics_Support_Bay,
  Shield_Battery,
  Khaydarin_Crystal_Form,
  Protoss_Temple,
  XelNaga_Temple,
  Mineral_Field,
  Vespene_Geyser,
  Warp_Gate,
  Psi_Disrupter,
  Power_Generator,
  Overmind_Cocoon,
  Dark_Swarm,
  None,
  UNKNOWN
};
enum FliterAttributeScalar
{
  HitPoints,
  InitialHitPoints,
  Shields,
  Energy,
  Resources,
  InitialResources,
  KillCount,
  GroundWeaponCooldown,
  AirWeaponCooldown,
  SpellCooldown,
  DefenseMatrixPoints,
  DefenseMatrixTimer,
  EnsnareTimer,
  IrradiateTimer,
  LockdownTimer,
  MaelstromTimer,
  PlagueTimer,
  RemoveTimer,
  StasisTimer,
  StimTimer,
  PositionX,
  PositionY,
  InitialPositionX,
  InitialPositionY,
  TilePositionX,
  TilePositionY,
  InitialTilePositionX,
  InitialTilePositionY,
  Angle,
  VelocityX,
  VelocityY,
  TargetPositionX,
  TargetPositionY,
  OrderTimer,
  RemainingBuildTime,
  RemainingTrainTime,
  TrainingQueueCount,
  LoadedUnitsCount,
  InterceptorCount,
  ScarabCount,
  SpiderMineCount,
  RemainingResearchTime,
  RemainingUpgradeTime,
  RallyPositionX,
  RallyPositionY
};

enum FilterAttributeUnit
{
  GetTarget,
  GetOrderTarget,
  GetBuildUnit,
  GetTransport,
  GetRallyUnit,
  GetAddon
};
enum FilterAttributeType
{
  GetType,
  GetInitialType,
  GetBuildType,
  GetTech,
  GetUpgrade
};

enum FilterAttributePosition
{
  GetPosition,
  GetInitialPosition,
  GetTargetPosition,
  GetRallyPosition
};

enum FilterAttributeTilePosition
{
  GetTilePosition,
  GetInitialTilePosition,
};
enum FilterAttributeOrder
{
  GetOrder,
  GetSecondaryOrder
};


class UnitGroup : public std::set<BWAPI::Unit*>
{
  public:
    static UnitGroup getUnitGroup(const std::set<BWAPI::Unit*> units)
    {
      UnitGroup u;
      u.insert(units.begin(), units.end());
      return u;
    }

    UnitGroup operator+(const UnitGroup& other) const;//union
    UnitGroup operator*(const UnitGroup& other) const;//intersection
    UnitGroup operator^(const UnitGroup& other) const;//symmetric difference
    UnitGroup operator-(const UnitGroup& other) const;//difference
    UnitGroup inRadius(double radius,BWAPI::Position position) const;
    UnitGroup inRegion(BWTA::Region* region) const;
    UnitGroup onlyNearestChokepoint(BWTA::Chokepoint* choke) const;
    UnitGroup onlyNearestBaseLocation(BWTA::BaseLocation* location) const;
    UnitGroup onlyNearestUnwalkablePolygon(BWTA::Polygon* polygon) const;
    UnitGroup operator()(int f1) const;
    UnitGroup operator()(int f1, int f2) const;
    UnitGroup operator()(int f1, int f2, int f3) const;
    UnitGroup operator()(int f1, int f2, int f3, int f4) const;
    UnitGroup operator()(int f1, int f2, int f3, int f4, int f5) const;
    UnitGroup operator()(FliterAttributeScalar a, const char* compare, double value) const;
    UnitGroup operator()(FliterAttributeScalar a, const char* compare, int value) const;
    UnitGroup operator()(BWAPI::Player* player) const;
    UnitGroup operator()(FilterAttributeUnit a, BWAPI::Unit* unit) const;
    UnitGroup operator()(FilterAttributeType a, BWAPI::UnitType type) const;
    UnitGroup operator()(FilterAttributeType a, BWAPI::TechType type) const;
    UnitGroup operator()(FilterAttributeType a, BWAPI::UpgradeType type) const;
    UnitGroup operator()(FilterAttributeOrder a, BWAPI::Order type) const;
    UnitGroup operator()(FilterAttributePosition a, BWAPI::Position position) const;
    UnitGroup operator()(FilterAttributeTilePosition a, BWAPI::TilePosition position) const;

    UnitGroup not(int f1) const;
    UnitGroup not(int f1, int f2) const;
    UnitGroup not(int f1, int f2, int f3) const;
    UnitGroup not(int f1, int f2, int f3, int f4) const;
    UnitGroup not(int f1, int f2, int f3, int f4, int f5) const;
    UnitGroup not(FliterAttributeScalar a, const char* compare, double value) const;
    UnitGroup not(FliterAttributeScalar a, const char* compare, int value) const;
    UnitGroup not(BWAPI::Player* player) const;
    UnitGroup not(FilterAttributeUnit a, BWAPI::Unit* unit) const;
    UnitGroup not(FilterAttributeType a, BWAPI::UnitType type) const;
    UnitGroup not(FilterAttributeType a, BWAPI::TechType type) const;
    UnitGroup not(FilterAttributeType a, BWAPI::UpgradeType type) const;
    UnitGroup not(FilterAttributeOrder a, BWAPI::Order type) const;
    UnitGroup not(FilterAttributePosition a, BWAPI::Position position) const;
    UnitGroup not(FilterAttributeTilePosition a, BWAPI::TilePosition position) const;

    UnitGroup& operator+=(const UnitGroup& other);//union
    UnitGroup& operator*=(const UnitGroup& other);//intersection
    UnitGroup& operator^=(const UnitGroup& other);//symmetric difference
    UnitGroup& operator-=(const UnitGroup& other);//difference
    BWAPI::Unit* getNearest(BWAPI::Position position) const;
    bool contains(BWAPI::Unit* u) const;
    BWAPI::Position getCenter() const;

    bool attack(BWAPI::Position position) const;
    bool attack(BWAPI::Unit* target) const;
    bool rightClick(BWAPI::Position position) const;
    bool rightClick(BWAPI::Unit* target) const;
    bool train(BWAPI::UnitType type) const;
    bool build(BWAPI::TilePosition position, BWAPI::UnitType type) const;
    bool buildAddon(BWAPI::UnitType type) const;
    bool research(BWAPI::TechType tech) const;
    bool upgrade(BWAPI::UpgradeType upgrade) const;
    bool stop() const;
    bool holdPosition() const;
    bool patrol(BWAPI::Position position) const;
    bool follow(BWAPI::Unit* target) const;
    bool setRallyPosition(BWAPI::Position target) const;
    bool setRallyUnit(BWAPI::Unit* target) const;
    bool repair(BWAPI::Unit* target) const;
    bool morph(BWAPI::UnitType type) const;
    bool burrow() const;
    bool unburrow() const;
    bool siege() const;
    bool unsiege() const;
    bool cloak() const;
    bool decloak() const;
    bool lift() const;
    bool land(BWAPI::TilePosition position) const;
    bool load(BWAPI::Unit* target) const;
    bool unload(BWAPI::Unit* target) const;
    bool unloadAll() const;
    bool unloadAll(BWAPI::Position position) const;
    bool cancelConstruction() const;
    bool haltConstruction() const;
    bool cancelMorph() const;
    bool cancelTrain() const;
    bool cancelTrain(int slot) const;
    bool cancelAddon() const;
    bool cancelResearch() const;
    bool cancelUpgrade() const;
    bool useTech(BWAPI::TechType tech) const;
    bool useTech(BWAPI::TechType tech, BWAPI::Position position) const;
    bool useTech(BWAPI::TechType tech, BWAPI::Unit* target) const;
};

