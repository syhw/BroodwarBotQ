#pragma once
#include <BWAPI.h>
#include <BWTA.h>
enum FilterFlag
{
  exists = 1,
  isAccelerating,
  isAttacking,
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
  isDetected,
  isEnsnared,
  isFollowing,
  isGatheringGas,
  isGatheringMinerals,
  isHallucination,
  isHoldingPosition,
  isIdle,
  isInterruptible,
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
  isStuck,
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
  isHero,
  isPowerup,
  isBeacon,
  isFlagBeacon,
  isSpecialBuilding,
  isSpell,

  Firebat,
  Ghost,
  Goliath,
  Marine,
  Medic,
  SCV,
  Siege_Tank,
  Vulture,
  Vulture_Spider_Mine,
  Battlecruiser,
  Dropship,
  Nuclear_Missile,
  Science_Vessel,
  Valkyrie,
  Wraith,
  Alan_Schezar,
  Alexei_Stukov,
  Arcturus_Mengsk,
  Edmund_Duke,
  Gerard_DuGalle,
  Gui_Montag,
  Hyperion,
  Jim_Raynor_Marine,
  Jim_Raynor_Vulture,
  Magellan,
  Norad_II,
  Samir_Duran,
  Sarah_Kerrigan,
  Tom_Kazansky,
  Civilian,
  Academy,
  Armory,
  Barracks,
  Bunker,
  Command_Center,
  Engineering_Bay,
  Factory,
  Missile_Turret,
  Refinery,
  Science_Facility,
  Starport,
  Supply_Depot,
  Comsat_Station,
  Control_Tower,
  Covert_Ops,
  Machine_Shop,
  Nuclear_Silo,
  Physics_Lab,
  Crashed_Norad_II,
  Ion_Cannon,
  Power_Generator,
  Psi_Disrupter,

  Archon,
  Dark_Archon,
  Dark_Templar,
  Dragoon,
  High_Templar,
  Probe,
  Reaver,
  Scarab,
  Zealot,
  Arbiter,
  Carrier,
  Corsair,
  Interceptor,
  Observer,
  Scout,
  Shuttle,
  Aldaris,
  Artanis,
  Danimoth,
  Hero_Dark_Templar,
  Fenix_Dragoon,
  Fenix_Zealot,
  Gantrithor,
  Mojo,
  Raszagal,
  Tassadar,
  Tassadar_Zeratul_Archon,
  Warbringer,
  Zeratul,
  Arbiter_Tribunal,
  Assimilator,
  Citadel_of_Adun,
  Cybernetics_Core,
  Fleet_Beacon,
  Forge,
  Gateway,
  Nexus,
  Observatory,
  Photon_Cannon,
  Pylon,
  Robotics_Facility,
  Robotics_Support_Bay,
  Shield_Battery,
  Stargate,
  Templar_Archives,
  Khaydarin_Crystal_Form,
  Protoss_Temple,
  Stasis_Cell_Prison,
  Warp_Gate,
  XelNaga_Temple,

  Broodling,
  Defiler,
  Drone,
  Egg,
  Hydralisk,
  Infested_Terran,
  Larva,
  Lurker,
  Lurker_Egg,
  Ultralisk,
  Zergling,
  Cocoon,
  Devourer,
  Guardian,
  Mutalisk,
  Overlord,
  Queen,
  Scourge,
  Devouring_One,
  Hunter_Killer,
  Infested_Duran,
  Infested_Kerrigan,
  Kukulza_Guardian,
  Kukulza_Mutalisk,
  Matriarch,
  Torrasque,
  Unclean_One,
  Yggdrasill,
  Creep_Colony,
  Defiler_Mound,
  Evolution_Chamber,
  Extractor,
  Greater_Spire,
  Hatchery,
  Hive,
  Hydralisk_Den,
  Infested_Command_Center,
  Lair,
  Nydus_Canal,
  Queens_Nest,
  Spawning_Pool,
  Spire,
  Spore_Colony,
  Sunken_Colony,
  Ultralisk_Cavern,
  Cerebrate,
  Cerebrate_Daggoth,
  Mature_Chrysalis,
  Overmind,
  Overmind_Cocoon,
  Overmind_With_Shell,

  Bengalaas,
  Kakaru,
  Ragnasaur,
  Rhynadon,
  Scantid,
  Ursadon,

  Mineral_Field,
  Vespene_Geyser,

  Dark_Swarm,
  Disruption_Web,
  Scanner_Sweep,

  Protoss_Beacon,
  Protoss_Flag_Beacon,
  Terran_Beacon,
  Terran_Flag_Beacon,
  Zerg_Beacon,
  Zerg_Flag_Beacon,

  Powerup_Data_Disk,
  Powerup_Flag,
  Powerup_Khalis_Crystal,
  Powerup_Khaydarin_Crystal,
  Powerup_Psi_Emitter,
  Powerup_Uraj_Crystal,
  Powerup_Young_Chrysalis,

  None,
  Unknown_Unit
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

    bool attackMove(BWAPI::Position position) const;
    bool attackUnit(BWAPI::Unit* target) const;
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
    bool setRallyPoint(BWAPI::Position target) const;
    bool setRallyPoint(BWAPI::Unit* target) const;
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

