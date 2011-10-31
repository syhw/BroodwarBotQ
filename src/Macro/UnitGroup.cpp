#include <PrecompiledHeader.h>
#include "Macro/UnitGroup.h"

using namespace BWAPI;
using namespace std;

bool passesFlag(Unit* u, int f)
{
  if (f<0)
    return !passesFlag(u,-f);
  switch(f)
  {
    case exists:
      if (u->exists()) return true;
    break;
    case isAccelerating:
      if (u->isAccelerating()) return true;
    break;
    case isAttacking:
      if (u->isAttacking()) return true;
    break;
    case isBeingConstructed:
      if (u->isBeingConstructed()) return true;
    break;
    case isBeingGathered:
      if (u->isBeingGathered()) return true;
    break;
    case isBeingHealed:
      if (u->isBeingHealed()) return true;
    break;
    case isBlind:
      if (u->isBlind()) return true;
    break;
    case isBraking:
      if (u->isBraking()) return true;
    break;
    case isBurrowed:
      if (u->isBurrowed()) return true;
    break;
    case isCarryingGas:
      if (u->isCarryingGas()) return true;
    break;
    case isCarryingMinerals:
      if (u->isCarryingMinerals()) return true;
    break;
    case isCloaked:
      if (u->isCloaked()) return true;
    break;
    case isCompleted:
      if (u->isCompleted()) return true;
    break;
    case isConstructing:
      if (u->isConstructing()) return true;
    break;
    case isDefenseMatrixed:
      if (u->isDefenseMatrixed()) return true;
    break;
    case isDetected:
      if (u->isDetected()) return true;
    break;
    case isEnsnared:
      if (u->isEnsnared()) return true;
    break;
    case isFollowing:
      if (u->isFollowing()) return true;
    break;
    case isGatheringGas:
      if (u->isGatheringGas()) return true;
    break;
    case isGatheringMinerals:
      if (u->isGatheringMinerals()) return true;
    break;
    case isHallucination:
      if (u->isHallucination()) return true;
    break;
    case isHoldingPosition:
      if (u->isHoldingPosition()) return true;
    break;
    case isIdle:
      if (u->isIdle()) return true;
    break;
    case isInterruptible:
      if (u->isInterruptible()) return true;
    break;
    case isIrradiated:
      if (u->isIrradiated()) return true;
    break;
    case isLifted:
      if (u->isLifted()) return true;
    break;
    case isLoaded:
      if (u->isLoaded()) return true;
    break;
    case isLockedDown:
      if (u->isLockedDown()) return true;
    break;
    case isMaelstrommed:
      if (u->isMaelstrommed()) return true;
    break;
    case isMorphing:
      if (u->isMorphing()) return true;
    break;
    case isMoving:
      if (u->isMoving()) return true;
    break;
    case isParasited:
      if (u->isParasited()) return true;
    break;
    case isPatrolling:
      if (u->isPatrolling()) return true;
    break;
    case isPlagued:
      if (u->isPlagued()) return true;
    break;
    case isRepairing:
      if (u->isRepairing()) return true;
    break;
    case isResearching:
      if (u->isResearching()) return true;
    break;
    case isSelected:
      if (u->isSelected()) return true;
    break;
    case isSieged:
      if (u->isSieged()) return true;
    break;
    case isStartingAttack:
      if (u->isStartingAttack()) return true;
    break;
    case isStasised:
      if (u->isStasised()) return true;
    break;
    case isStimmed:
      if (u->isStimmed()) return true;
    break;
    case isStuck:
      if (u->isStuck()) return true;
    break;
    case isTraining:
      if (u->isTraining()) return true;
    break;
    case isUnderStorm:
      if (u->isUnderStorm()) return true;
    break;
    case isUnpowered:
      if (u->isUnpowered()) return true;
    break;
    case isUpgrading:
      if (u->isUpgrading()) return true;
    break;
    case isVisible:
      if (u->isVisible()) return true;
    break;
    case canProduce:
      if (u->getType().canProduce()) return true;
    break;
    case canAttack:
      if (u->getType().canAttack()) return true;
    break;
    case canMove:
      if (u->getType().canMove()) return true;
    break;
    case isFlyer:
      if (u->getType().isFlyer()) return true;
    break;
    case regeneratesHP:
      if (u->getType().regeneratesHP()) return true;
    break;
    case isSpellcaster:
      if (u->getType().isSpellcaster()) return true;
    break;
    case hasPermanentCloak:
      if (u->getType().hasPermanentCloak()) return true;
    break;
    case isInvincible:
      if (u->getType().isInvincible()) return true;
    break;
    case isOrganic:
      if (u->getType().isOrganic()) return true;
    break;
    case isMechanical:
      if (u->getType().isMechanical()) return true;
    break;
    case isRobotic:
      if (u->getType().isRobotic()) return true;
    break;
    case isDetector:
      if (u->getType().isDetector()) return true;
    break;
    case isResourceContainer:
      if (u->getType().isResourceContainer()) return true;
    break;
    case isResourceDepot:
      if (u->getType().isResourceDepot()) return true;
    break;
    case isRefinery:
      if (u->getType().isRefinery()) return true;
    break;
    case isWorker:
      if (u->getType().isWorker()) return true;
    break;
    case requiresPsi:
      if (u->getType().requiresPsi()) return true;
    break;
    case requiresCreep:
      if (u->getType().requiresCreep()) return true;
    break;
    case isTwoUnitsInOneEgg:
      if (u->getType().isTwoUnitsInOneEgg()) return true;
    break;
    case isBurrowable:
      if (u->getType().isBurrowable()) return true;
    break;
    case isCloakable:
      if (u->getType().isCloakable()) return true;
    break;
    case isBuilding:
      if (u->getType().isBuilding()) return true;
    break;
    case isAddon:
      if (u->getType().isAddon()) return true;
    break;
    case isFlyingBuilding:
      if (u->getType().isFlyingBuilding()) return true;
    break;
    case isNeutral:
      if (u->getType().isNeutral()) return true;
    break;
    case isHero:
      if (u->getType().isHero()) return true;
    break;
    case isPowerup:
      if (u->getType().isPowerup()) return true;
    break;
    case isBeacon:
      if (u->getType().isBeacon()) return true;
    break;
    case isFlagBeacon:
      if (u->getType().isFlagBeacon()) return true;
    break;
    case isSpecialBuilding:
      if (u->getType().isSpecialBuilding()) return true;
    break;
    case isSpell:
      if (u->getType().isSpell()) return true;
    break;
    case Firebat:
      if(u->getType()==UnitTypes::Terran_Firebat) return true;
    break;
    case Ghost:
      if(u->getType()==UnitTypes::Terran_Ghost) return true;
    break;
    case Goliath:
      if(u->getType()==UnitTypes::Terran_Goliath) return true;
    break;
    case Marine:
      if(u->getType()==UnitTypes::Terran_Marine) return true;
    break;
    case Medic:
      if(u->getType()==UnitTypes::Terran_Medic) return true;
    break;
    case SCV:
      if(u->getType()==UnitTypes::Terran_SCV) return true;
    break;
    case Siege_Tank:
      if(u->getType()==UnitTypes::Terran_Siege_Tank_Tank_Mode) return true;
      if(u->getType()==UnitTypes::Terran_Siege_Tank_Siege_Mode) return true;
    break;
    case Vulture:
      if(u->getType()==UnitTypes::Terran_Vulture) return true;
    break;
    case Vulture_Spider_Mine:
      if(u->getType()==UnitTypes::Terran_Vulture_Spider_Mine) return true;
    break;
    case Battlecruiser:
      if(u->getType()==UnitTypes::Terran_Battlecruiser) return true;
    break;
    case Dropship:
      if(u->getType()==UnitTypes::Terran_Dropship) return true;
    break;
    case Nuclear_Missile:
      if(u->getType()==UnitTypes::Terran_Nuclear_Missile) return true;
    break;
    case Science_Vessel:
      if(u->getType()==UnitTypes::Terran_Science_Vessel) return true;
    break;
    case Valkyrie:
      if(u->getType()==UnitTypes::Terran_Valkyrie) return true;
    break;
    case Wraith:
      if(u->getType()==UnitTypes::Terran_Wraith) return true;
    break;
    case Alan_Schezar:
      if(u->getType()==UnitTypes::Hero_Alan_Schezar) return true;
    break;
    case Alexei_Stukov:
      if(u->getType()==UnitTypes::Hero_Alexei_Stukov) return true;
    break;
    case Arcturus_Mengsk:
      if(u->getType()==UnitTypes::Hero_Arcturus_Mengsk) return true;
    break;
    case Edmund_Duke:
      if(u->getType()==UnitTypes::Hero_Edmund_Duke_Siege_Mode) return true;
      if(u->getType()==UnitTypes::Hero_Edmund_Duke_Tank_Mode) return true;
    break;
    case Gerard_DuGalle:
      if(u->getType()==UnitTypes::Hero_Gerard_DuGalle) return true;
    break;
    case Gui_Montag:
      if(u->getType()==UnitTypes::Hero_Gui_Montag) return true;
    break;
    case Hyperion:
      if(u->getType()==UnitTypes::Hero_Hyperion) return true;
    break;
    case Jim_Raynor_Marine:
      if(u->getType()==UnitTypes::Hero_Jim_Raynor_Marine) return true;
    break;
    case Jim_Raynor_Vulture:
      if(u->getType()==UnitTypes::Hero_Jim_Raynor_Vulture) return true;
    break;
    case Magellan:
      if(u->getType()==UnitTypes::Hero_Magellan) return true;
    break;
    case Norad_II:
      if(u->getType()==UnitTypes::Hero_Norad_II) return true;
    break;
    case Samir_Duran:
      if(u->getType()==UnitTypes::Hero_Samir_Duran) return true;
    break;
    case Sarah_Kerrigan:
      if(u->getType()==UnitTypes::Hero_Sarah_Kerrigan) return true;
    break;
    case Tom_Kazansky:
      if(u->getType()==UnitTypes::Hero_Tom_Kazansky) return true;
    break;
    case Civilian:
      if(u->getType()==UnitTypes::Terran_Civilian) return true;
    break;
    case Academy:
      if(u->getType()==UnitTypes::Terran_Academy) return true;
    break;
    case Armory:
      if(u->getType()==UnitTypes::Terran_Armory) return true;
    break;
    case Barracks:
      if(u->getType()==UnitTypes::Terran_Barracks) return true;
    break;
    case Bunker:
      if(u->getType()==UnitTypes::Terran_Bunker) return true;
    break;
    case Command_Center:
      if(u->getType()==UnitTypes::Terran_Command_Center) return true;
    break;
    case Engineering_Bay:
      if(u->getType()==UnitTypes::Terran_Engineering_Bay) return true;
    break;
    case Factory:
      if(u->getType()==UnitTypes::Terran_Factory) return true;
    break;
    case Missile_Turret:
      if(u->getType()==UnitTypes::Terran_Missile_Turret) return true;
    break;
    case Refinery:
      if(u->getType()==UnitTypes::Terran_Refinery) return true;
    break;
    case Science_Facility:
      if(u->getType()==UnitTypes::Terran_Science_Facility) return true;
    break;
    case Starport:
      if(u->getType()==UnitTypes::Terran_Starport) return true;
    break;
    case Supply_Depot:
      if(u->getType()==UnitTypes::Terran_Supply_Depot) return true;
    break;
    case Comsat_Station:
      if(u->getType()==UnitTypes::Terran_Comsat_Station) return true;
    break;
    case Control_Tower:
      if(u->getType()==UnitTypes::Terran_Control_Tower) return true;
    break;
    case Covert_Ops:
      if(u->getType()==UnitTypes::Terran_Covert_Ops) return true;
    break;
    case Machine_Shop:
      if(u->getType()==UnitTypes::Terran_Machine_Shop) return true;
    break;
    case Nuclear_Silo:
      if(u->getType()==UnitTypes::Terran_Nuclear_Silo) return true;
    break;
    case Physics_Lab:
      if(u->getType()==UnitTypes::Terran_Physics_Lab) return true;
    break;
    case Crashed_Norad_II:
      if(u->getType()==UnitTypes::Special_Crashed_Norad_II) return true;
    break;
    case Ion_Cannon:
      if(u->getType()==UnitTypes::Special_Ion_Cannon) return true;
    break;
    case Power_Generator:
      if(u->getType()==UnitTypes::Special_Power_Generator) return true;
    break;
    case Psi_Disrupter:
      if(u->getType()==UnitTypes::Special_Psi_Disrupter) return true;
    break;
    case Archon:
      if(u->getType()==UnitTypes::Protoss_Archon) return true;
    break;
    case Dark_Archon:
      if(u->getType()==UnitTypes::Protoss_Dark_Archon) return true;
    break;
    case Dark_Templar:
      if(u->getType()==UnitTypes::Protoss_Dark_Templar) return true;
    break;
    case Dragoon:
      if(u->getType()==UnitTypes::Protoss_Dragoon) return true;
    break;
    case High_Templar:
      if(u->getType()==UnitTypes::Protoss_High_Templar) return true;
    break;
    case Probe:
      if(u->getType()==UnitTypes::Protoss_Probe) return true;
    break;
    case Reaver:
      if(u->getType()==UnitTypes::Protoss_Reaver) return true;
    break;
    case Scarab:
      if(u->getType()==UnitTypes::Protoss_Scarab) return true;
    break;
    case Zealot:
      if(u->getType()==UnitTypes::Protoss_Zealot) return true;
    break;
    case Arbiter:
      if(u->getType()==UnitTypes::Protoss_Arbiter) return true;
    break;
    case Carrier:
      if(u->getType()==UnitTypes::Protoss_Carrier) return true;
    break;
    case Corsair:
      if(u->getType()==UnitTypes::Protoss_Corsair) return true;
    break;
    case Interceptor:
      if(u->getType()==UnitTypes::Protoss_Interceptor) return true;
    break;
    case Observer:
      if(u->getType()==UnitTypes::Protoss_Observer) return true;
    break;
    case Scout:
      if(u->getType()==UnitTypes::Protoss_Scout) return true;
    break;
    case Shuttle:
      if(u->getType()==UnitTypes::Protoss_Shuttle) return true;
    break;
    case Aldaris:
      if(u->getType()==UnitTypes::Hero_Aldaris) return true;
    break;
    case Artanis:
      if(u->getType()==UnitTypes::Hero_Artanis) return true;
    break;
    case Danimoth:
      if(u->getType()==UnitTypes::Hero_Danimoth) return true;
    break;
    case Hero_Dark_Templar:
      if(u->getType()==UnitTypes::Hero_Dark_Templar) return true;
    break;
    case Fenix_Dragoon:
      if(u->getType()==UnitTypes::Hero_Fenix_Dragoon) return true;
    break;
    case Fenix_Zealot:
      if(u->getType()==UnitTypes::Hero_Fenix_Zealot) return true;
    break;
    case Gantrithor:
      if(u->getType()==UnitTypes::Hero_Gantrithor) return true;
    break;
    case Mojo:
      if(u->getType()==UnitTypes::Hero_Mojo) return true;
    break;
    case Raszagal:
      if(u->getType()==UnitTypes::Hero_Raszagal) return true;
    break;
    case Tassadar:
      if(u->getType()==UnitTypes::Hero_Tassadar) return true;
    break;
    case Tassadar_Zeratul_Archon:
      if(u->getType()==UnitTypes::Hero_Tassadar_Zeratul_Archon) return true;
    break;
    case Warbringer:
      if(u->getType()==UnitTypes::Hero_Warbringer) return true;
    break;
    case Zeratul:
      if(u->getType()==UnitTypes::Hero_Zeratul) return true;
    break;
    case Arbiter_Tribunal:
      if(u->getType()==UnitTypes::Protoss_Arbiter_Tribunal) return true;
    break;
    case Assimilator:
      if(u->getType()==UnitTypes::Protoss_Assimilator) return true;
    break;
    case Citadel_of_Adun:
      if(u->getType()==UnitTypes::Protoss_Citadel_of_Adun) return true;
    break;
    case Cybernetics_Core:
      if(u->getType()==UnitTypes::Protoss_Cybernetics_Core) return true;
    break;
    case Fleet_Beacon:
      if(u->getType()==UnitTypes::Protoss_Fleet_Beacon) return true;
    break;
    case Forge:
      if(u->getType()==UnitTypes::Protoss_Forge) return true;
    break;
    case Gateway:
      if(u->getType()==UnitTypes::Protoss_Gateway) return true;
    break;
    case Nexus:
      if(u->getType()==UnitTypes::Protoss_Nexus) return true;
    break;
    case Observatory:
      if(u->getType()==UnitTypes::Protoss_Observatory) return true;
    break;
    case Photon_Cannon:
      if(u->getType()==UnitTypes::Protoss_Photon_Cannon) return true;
    break;
    case Pylon:
      if(u->getType()==UnitTypes::Protoss_Pylon) return true;
    break;
    case Robotics_Facility:
      if(u->getType()==UnitTypes::Protoss_Robotics_Facility) return true;
    break;
    case Robotics_Support_Bay:
      if(u->getType()==UnitTypes::Protoss_Robotics_Support_Bay) return true;
    break;
    case Shield_Battery:
      if(u->getType()==UnitTypes::Protoss_Shield_Battery) return true;
    break;
    case Stargate:
      if(u->getType()==UnitTypes::Protoss_Stargate) return true;
    break;
    case Templar_Archives:
      if(u->getType()==UnitTypes::Protoss_Templar_Archives) return true;
    break;
    case Khaydarin_Crystal_Form:
      if(u->getType()==UnitTypes::Special_Khaydarin_Crystal_Form) return true;
    break;
    case Protoss_Temple:
      if(u->getType()==UnitTypes::Special_Protoss_Temple) return true;
    break;
    case Stasis_Cell_Prison:
      if(u->getType()==UnitTypes::Special_Stasis_Cell_Prison) return true;
    break;
    case Warp_Gate:
      if(u->getType()==UnitTypes::Special_Warp_Gate) return true;
    break;
    case XelNaga_Temple:
      if(u->getType()==UnitTypes::Special_XelNaga_Temple) return true;
    break;
    case Broodling:
      if(u->getType()==UnitTypes::Zerg_Broodling) return true;
    break;
    case Defiler:
      if(u->getType()==UnitTypes::Zerg_Defiler) return true;
    break;
    case Drone:
      if(u->getType()==UnitTypes::Zerg_Drone) return true;
    break;
    case Egg:
      if(u->getType()==UnitTypes::Zerg_Egg) return true;
    break;
    case Hydralisk:
      if(u->getType()==UnitTypes::Zerg_Hydralisk) return true;
    break;
    case Infested_Terran:
      if(u->getType()==UnitTypes::Zerg_Infested_Terran) return true;
    break;
    case Larva:
      if(u->getType()==UnitTypes::Zerg_Larva) return true;
    break;
    case Lurker:
      if(u->getType()==UnitTypes::Zerg_Lurker) return true;
    break;
    case Lurker_Egg:
      if(u->getType()==UnitTypes::Zerg_Lurker_Egg) return true;
    break;
    case Ultralisk:
      if(u->getType()==UnitTypes::Zerg_Ultralisk) return true;
    break;
    case Zergling:
      if(u->getType()==UnitTypes::Zerg_Zergling) return true;
    break;
    case Cocoon:
      if(u->getType()==UnitTypes::Zerg_Cocoon) return true;
    break;
    case Devourer:
      if(u->getType()==UnitTypes::Zerg_Devourer) return true;
    break;
    case Guardian:
      if(u->getType()==UnitTypes::Zerg_Guardian) return true;
    break;
    case Mutalisk:
      if(u->getType()==UnitTypes::Zerg_Mutalisk) return true;
    break;
    case Overlord:
      if(u->getType()==UnitTypes::Zerg_Overlord) return true;
    break;
    case Queen:
      if(u->getType()==UnitTypes::Zerg_Queen) return true;
    break;
    case Scourge:
      if(u->getType()==UnitTypes::Zerg_Scourge) return true;
    break;
    case Devouring_One:
      if(u->getType()==UnitTypes::Hero_Devouring_One) return true;
    break;
    case Hunter_Killer:
      if(u->getType()==UnitTypes::Hero_Hunter_Killer) return true;
    break;
    case Infested_Duran:
      if(u->getType()==UnitTypes::Hero_Infested_Duran) return true;
    break;
    case Infested_Kerrigan:
      if(u->getType()==UnitTypes::Hero_Infested_Kerrigan) return true;
    break;
    case Kukulza_Guardian:
      if(u->getType()==UnitTypes::Hero_Kukulza_Guardian) return true;
    break;
    case Kukulza_Mutalisk:
      if(u->getType()==UnitTypes::Hero_Kukulza_Mutalisk) return true;
    break;
    case Matriarch:
      if(u->getType()==UnitTypes::Hero_Matriarch) return true;
    break;
    case Torrasque:
      if(u->getType()==UnitTypes::Hero_Torrasque) return true;
    break;
    case Unclean_One:
      if(u->getType()==UnitTypes::Hero_Unclean_One) return true;
    break;
    case Yggdrasill:
      if(u->getType()==UnitTypes::Hero_Yggdrasill) return true;
    break;
    case Creep_Colony:
      if(u->getType()==UnitTypes::Zerg_Creep_Colony) return true;
    break;
    case Defiler_Mound:
      if(u->getType()==UnitTypes::Zerg_Defiler_Mound) return true;
    break;
    case Evolution_Chamber:
      if(u->getType()==UnitTypes::Zerg_Evolution_Chamber) return true;
    break;
    case Extractor:
      if(u->getType()==UnitTypes::Zerg_Extractor) return true;
    break;
    case Greater_Spire:
      if(u->getType()==UnitTypes::Zerg_Greater_Spire) return true;
    break;
    case Hatchery:
      if(u->getType()==UnitTypes::Zerg_Hatchery) return true;
    break;
    case Hive:
      if(u->getType()==UnitTypes::Zerg_Hive) return true;
    break;
    case Hydralisk_Den:
      if(u->getType()==UnitTypes::Zerg_Hydralisk_Den) return true;
    break;
    case Infested_Command_Center:
      if(u->getType()==UnitTypes::Zerg_Infested_Command_Center) return true;
    break;
    case Lair:
      if(u->getType()==UnitTypes::Zerg_Lair) return true;
    break;
    case Nydus_Canal:
      if(u->getType()==UnitTypes::Zerg_Nydus_Canal) return true;
    break;
    case Queens_Nest:
      if(u->getType()==UnitTypes::Zerg_Queens_Nest) return true;
    break;
    case Spawning_Pool:
      if(u->getType()==UnitTypes::Zerg_Spawning_Pool) return true;
    break;
    case Spire:
      if(u->getType()==UnitTypes::Zerg_Spire) return true;
    break;
    case Spore_Colony:
      if(u->getType()==UnitTypes::Zerg_Spore_Colony) return true;
    break;
    case Sunken_Colony:
      if(u->getType()==UnitTypes::Zerg_Sunken_Colony) return true;
    break;
    case Ultralisk_Cavern:
      if(u->getType()==UnitTypes::Zerg_Ultralisk_Cavern) return true;
    break;
    case Cerebrate:
      if(u->getType()==UnitTypes::Special_Cerebrate) return true;
    break;
    case Cerebrate_Daggoth:
      if(u->getType()==UnitTypes::Special_Cerebrate_Daggoth) return true;
    break;
    case Mature_Chrysalis:
      if(u->getType()==UnitTypes::Special_Mature_Chrysalis) return true;
    break;
    case Overmind:
      if(u->getType()==UnitTypes::Special_Overmind) return true;
    break;
    case Overmind_Cocoon:
      if(u->getType()==UnitTypes::Special_Overmind_Cocoon) return true;
    break;
    case Overmind_With_Shell:
      if(u->getType()==UnitTypes::Special_Overmind_With_Shell) return true;
    break;
    case Bengalaas:
      if(u->getType()==UnitTypes::Critter_Bengalaas) return true;
    break;
    case Kakaru:
      if(u->getType()==UnitTypes::Critter_Kakaru) return true;
    break;
    case Ragnasaur:
      if(u->getType()==UnitTypes::Critter_Ragnasaur) return true;
    break;
    case Rhynadon:
      if(u->getType()==UnitTypes::Critter_Rhynadon) return true;
    break;
    case Scantid:
      if(u->getType()==UnitTypes::Critter_Scantid) return true;
    break;
    case Ursadon:
      if(u->getType()==UnitTypes::Critter_Ursadon) return true;
    break;
    case Mineral_Field:
      if(u->getType()==UnitTypes::Resource_Mineral_Field) return true;
    break;
    case Vespene_Geyser:
      if(u->getType()==UnitTypes::Resource_Vespene_Geyser) return true;
    break;
    case Dark_Swarm:
      if(u->getType()==UnitTypes::Spell_Dark_Swarm) return true;
    break;
    case Disruption_Web:
      if(u->getType()==UnitTypes::Spell_Disruption_Web) return true;
    break;
    case Scanner_Sweep:
      if(u->getType()==UnitTypes::Spell_Scanner_Sweep) return true;
    break;
    case Protoss_Beacon:
      if(u->getType()==UnitTypes::Special_Protoss_Beacon) return true;
    break;
    case Protoss_Flag_Beacon:
      if(u->getType()==UnitTypes::Special_Protoss_Flag_Beacon) return true;
    break;
    case Terran_Beacon:
      if(u->getType()==UnitTypes::Special_Terran_Beacon) return true;
    break;
    case Terran_Flag_Beacon:
      if(u->getType()==UnitTypes::Special_Terran_Flag_Beacon) return true;
    break;
    case Zerg_Beacon:
      if(u->getType()==UnitTypes::Special_Zerg_Beacon) return true;
    break;
    case Zerg_Flag_Beacon:
      if(u->getType()==UnitTypes::Special_Zerg_Flag_Beacon) return true;
    break;
    case Powerup_Data_Disk:
      if(u->getType()==UnitTypes::Powerup_Data_Disk) return true;
    break;
    case Powerup_Flag:
      if(u->getType()==UnitTypes::Powerup_Flag) return true;
    break;
    case Powerup_Khalis_Crystal:
      if(u->getType()==UnitTypes::Powerup_Khalis_Crystal) return true;
    break;
    case Powerup_Khaydarin_Crystal:
      if(u->getType()==UnitTypes::Powerup_Khaydarin_Crystal) return true;
    break;
    case Powerup_Psi_Emitter:
      if(u->getType()==UnitTypes::Powerup_Psi_Emitter) return true;
    break;
    case Powerup_Uraj_Crystal:
      if(u->getType()==UnitTypes::Powerup_Uraj_Crystal) return true;
    break;
    case Powerup_Young_Chrysalis:
      if(u->getType()==UnitTypes::Powerup_Young_Chrysalis) return true;
    break;
    case None:
      if(u->getType()==UnitTypes::None) return true;
    break;
    case Unknown_Unit:
      if(u->getType()==UnitTypes::Unknown) return true;
    break;
  }
  return false;
}

double getAttribute(Unit* u, FliterAttributeScalar a)
{
  switch(a)
  {
    case HitPoints:
      return u->getHitPoints();
    break;
    case InitialHitPoints:
      return u->getInitialHitPoints();
    break;
    case Shields:
      return u->getShields();
    break;
    case Energy:
      return u->getEnergy();
    break;
    case Resources:
      return u->getResources();
    break;
    case InitialResources:
      return u->getInitialResources();
    break;
    case KillCount:
      return u->getKillCount();
    break;
    case GroundWeaponCooldown:
      return u->getGroundWeaponCooldown();
    break;
    case AirWeaponCooldown:
      return u->getAirWeaponCooldown();
    break;
    case SpellCooldown:
      return u->getSpellCooldown();
    break;
    case DefenseMatrixPoints:
      return u->getDefenseMatrixPoints();
    break;
    case DefenseMatrixTimer:
      return u->getDefenseMatrixTimer();
    break;
    case EnsnareTimer:
      return u->getEnsnareTimer();
    break;
    case IrradiateTimer:
      return u->getIrradiateTimer();
    break;
    case LockdownTimer:
      return u->getLockdownTimer();
    break;
    case MaelstromTimer:
      return u->getMaelstromTimer();
    break;
    case PlagueTimer:
      return u->getPlagueTimer();
    break;
    case RemoveTimer:
      return u->getRemoveTimer();
    break;
    case StasisTimer:
      return u->getStasisTimer();
    break;
    case StimTimer:
      return u->getStimTimer();
    break;
    case PositionX:
      return u->getPosition().x();
    break;
    case PositionY:
      return u->getPosition().y();
    break;
    case InitialPositionX:
      return u->getInitialPosition().x();
    break;
    case InitialPositionY:
      return u->getInitialPosition().y();
    break;
    case TilePositionX:
      return u->getTilePosition().x();
    break;
    case TilePositionY:
      return u->getTilePosition().y();
    break;
    case InitialTilePositionX:
      return u->getInitialTilePosition().x();
    break;
    case InitialTilePositionY:
      return u->getInitialTilePosition().y();
    break;
    case Angle:
      return u->getAngle();
    break;
    case VelocityX:
      return u->getVelocityX();
    break;
    case VelocityY:
      return u->getVelocityY();
    break;
    case TargetPositionX:
      return u->getTargetPosition().x();
    break;
    case TargetPositionY:
      return u->getTargetPosition().y();
    break;
    case OrderTimer:
      return u->getOrderTimer();
    break;
    case RemainingBuildTime:
      return u->getRemainingBuildTime();
    break;
    case RemainingTrainTime:
      return u->getRemainingTrainTime();
    break;
    case TrainingQueueCount:
      return u->getTrainingQueue().size();
    break;
    case LoadedUnitsCount:
      return u->getLoadedUnits().size();
    break;
    case InterceptorCount:
      return u->getInterceptorCount();
    break;
    case ScarabCount:
      return u->getScarabCount();
    break;
    case SpiderMineCount:
      return u->getSpiderMineCount();
    break;
    case RemainingResearchTime:
      return u->getRemainingResearchTime();
    break;
    case RemainingUpgradeTime:
      return u->getRemainingUpgradeTime();
    break;
    case RallyPositionX:
      return u->getRallyPosition().x();
    break;
    case RallyPositionY:
      return u->getRallyPosition().y();
    break;
  }
  return 0;
}

Unit* getUnit(Unit* u,FilterAttributeUnit a)
{
  switch(a)
  {
    case GetTarget:
      return u->getTarget();
    break;
    case GetOrderTarget:
      return u->getOrderTarget();
    break;
    case GetBuildUnit:
      return u->getBuildUnit();
    break;
    case GetTransport:
      return u->getTransport();
    break;
    case GetRallyUnit:
      return u->getRallyUnit();
    break;
    case GetAddon:
      return u->getAddon();
    break;
  }
  return u;
}
UnitGroup UnitGroup::operator+(const UnitGroup& other) const
{
  UnitGroup result=*this;
  result+=other;
  return result;
}
UnitGroup UnitGroup::operator*(const UnitGroup& other) const
{
  UnitGroup result=*this;
  result*=other;
  return result;
}
UnitGroup UnitGroup::operator^(const UnitGroup& other) const
{
  UnitGroup result=*this;
  result^=other;
  return result;
}
UnitGroup UnitGroup::operator-(const UnitGroup& other) const
{
  UnitGroup result=*this;
  result-=other;
  return result;
}

UnitGroup UnitGroup::operator()(int f1) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (passesFlag(*i,f1))
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::operator()(int f1, int f2) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (passesFlag(*i,f1) || passesFlag(*i,f2))
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::operator()(int f1, int f2, int f3) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (passesFlag(*i,f1) || passesFlag(*i,f2) || passesFlag(*i,f3))
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::operator()(int f1, int f2, int f3, int f4) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (passesFlag(*i,f1) || passesFlag(*i,f2) || passesFlag(*i,f3) || passesFlag(*i,f4))
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::operator()(int f1, int f2, int f3, int f4, int f5) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (passesFlag(*i,f1) || passesFlag(*i,f2) || passesFlag(*i,f3) || passesFlag(*i,f4) || passesFlag(*i,f5))
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::operator()(FliterAttributeScalar a, const char* compare, double value) const
{
  UnitGroup result;
  string cmp(compare);
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    double val=getAttribute(*i,a);
    bool passes=false;
    if (cmp=="=" || cmp=="==" || cmp=="<=" || cmp==">=")
      if (val==value)
        passes=true;
    if (cmp=="<" || cmp=="<=" || cmp=="!=" || cmp=="<>")
      if (val<value)
        passes=true;
    if (cmp==">" || cmp==">=" || cmp=="!=" || cmp=="<>")
      if (val>value)
        passes=true;
    if (passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::operator()(FliterAttributeScalar a, const char* compare, int value) const
{
  UnitGroup result;
  string cmp(compare);
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    int val=(int)getAttribute(*i,a);
    bool passes=false;
    if (cmp=="=" || cmp=="==" || cmp=="<=" || cmp==">=")
      if (val==value)
        passes=true;
    if (cmp=="<" || cmp=="<=" || cmp=="!=" || cmp=="<>")
      if (val<value)
        passes=true;
    if (cmp==">" || cmp==">=" || cmp=="!=" || cmp=="<>")
      if (val>value)
        passes=true;
    if (passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::operator()(BWAPI::Player* player) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if ((*i)->getPlayer()==player)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::operator()(FilterAttributeUnit a, BWAPI::Unit* unit) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    Unit* target=getUnit(*i,a);
    if (target==unit)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::operator()(FilterAttributeType a, BWAPI::UnitType type) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    bool passes=false;
    if (a==GetType)
      if ((*i)->getType()==type)
        passes=true;
    if (a==GetInitialType)
      if ((*i)->getInitialType()==type)
        passes=true;
    if (a==GetBuildType)
      if ((*i)->getBuildType()==type)
        passes=true;
    if (passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::operator()(FilterAttributeType a, BWAPI::TechType type) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    bool passes=false;
    if (a==GetTech)
      if ((*i)->getTech()==type)
        passes=true;
    if (passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::operator()(FilterAttributeOrder a, BWAPI::Order type) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    bool passes=false;
    if (a==GetOrder)
      if ((*i)->getOrder()==type)
        passes=true;
    if (a==GetSecondaryOrder)
      if ((*i)->getSecondaryOrder()==type)
        passes=true;
    if (passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::operator()(FilterAttributeType a, BWAPI::UpgradeType type) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    bool passes=false;
    if (a==GetUpgrade)
      if ((*i)->getUpgrade()==type)
        passes=true;
    if (passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::operator()(FilterAttributePosition a, BWAPI::Position position) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    bool passes=false;
    if (a==GetPosition)
      if ((*i)->getPosition()==position)
        passes=true;
    if (a==GetInitialPosition)
      if ((*i)->getInitialPosition()==position)
        passes=true;
    if (a==GetTargetPosition)
      if ((*i)->getTargetPosition()==position)
        passes=true;
    if (a==GetRallyPosition)
      if ((*i)->getRallyPosition()==position)
        passes=true;
    if (passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::operator()(FilterAttributeTilePosition a, BWAPI::TilePosition position) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    bool passes=false;
    if (a==GetPosition)
      if ((*i)->getTilePosition()==position)
        passes=true;
    if (a==GetInitialPosition)
      if ((*i)->getInitialTilePosition()==position)
        passes=true;
    if (passes)
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::not(int f1) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (!(passesFlag(*i,f1)))
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::not(int f1, int f2) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (!(passesFlag(*i,f1) || passesFlag(*i,f2)))
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::not(int f1, int f2, int f3) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (!(passesFlag(*i,f1) || passesFlag(*i,f2) || passesFlag(*i,f3)))
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::not(int f1, int f2, int f3, int f4) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (!(passesFlag(*i,f1) || passesFlag(*i,f2) || passesFlag(*i,f3) || passesFlag(*i,f4)))
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::not(int f1, int f2, int f3, int f4, int f5) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (!(passesFlag(*i,f1) || passesFlag(*i,f2) || passesFlag(*i,f3) || passesFlag(*i,f4) || passesFlag(*i,f5)))
      result.insert(*i);
  }
  return result;
}


UnitGroup UnitGroup::not(FliterAttributeScalar a, const char* compare, double value) const
{
  UnitGroup result;
  string cmp(compare);
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    double val=getAttribute(*i,a);
    bool passes=false;
    if (cmp=="=" || cmp=="==" || cmp=="<=" || cmp==">=")
      if (val==value)
        passes=true;
    if (cmp=="<" || cmp=="<=" || cmp=="!=" || cmp=="<>")
      if (val<value)
        passes=true;
    if (cmp==">" || cmp==">=" || cmp=="!=" || cmp=="<>")
      if (val>value)
        passes=true;
    if (passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::not(FliterAttributeScalar a, const char* compare, int value) const
{
  UnitGroup result;
  string cmp(compare);
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    int val=(int)getAttribute(*i,a);
    bool passes=false;
    if (cmp=="=" || cmp=="==" || cmp=="<=" || cmp==">=")
      if (val==value)
        passes=true;
    if (cmp=="<" || cmp=="<=" || cmp=="!=" || cmp=="<>")
      if (val<value)
        passes=true;
    if (cmp==">" || cmp==">=" || cmp=="!=" || cmp=="<>")
      if (val>value)
        passes=true;
    if (passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::not(BWAPI::Player* player) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if ((*i)->getPlayer()!=player)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::not(FilterAttributeUnit a, BWAPI::Unit* unit) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    Unit* target=getUnit(*i,a);
    if (target!=unit)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::not(FilterAttributeType a, BWAPI::UnitType type) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    bool passes=false;
    if (a==GetType)
      if ((*i)->getType()==type)
        passes=true;
    if (a==GetInitialType)
      if ((*i)->getInitialType()==type)
        passes=true;
    if (a==GetBuildType)
      if ((*i)->getBuildType()==type)
        passes=true;
    if (!passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::not(FilterAttributeType a, BWAPI::TechType type) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    bool passes=false;
    if (a==GetTech)
      if ((*i)->getTech()==type)
        passes=true;
    if (!passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::not(FilterAttributeOrder a, BWAPI::Order type) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    bool passes=false;
    if (a==GetOrder)
      if ((*i)->getOrder()==type)
        passes=true;
    if (a==GetSecondaryOrder)
      if ((*i)->getSecondaryOrder()==type)
        passes=true;
    if (!passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::not(FilterAttributeType a, BWAPI::UpgradeType type) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    bool passes=false;
    if (a==GetUpgrade)
      if ((*i)->getUpgrade()==type)
        passes=true;
    if (!passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::not(FilterAttributePosition a, BWAPI::Position position) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    bool passes=false;
    if (a==GetPosition)
      if ((*i)->getPosition()==position)
        passes=true;
    if (a==GetInitialPosition)
      if ((*i)->getInitialPosition()==position)
        passes=true;
    if (a==GetTargetPosition)
      if ((*i)->getTargetPosition()==position)
        passes=true;
    if (a==GetRallyPosition)
      if ((*i)->getRallyPosition()==position)
        passes=true;
    if (!passes)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::not(FilterAttributeTilePosition a, BWAPI::TilePosition position) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    bool passes=false;
    if (a==GetPosition)
      if ((*i)->getTilePosition()==position)
        passes=true;
    if (a==GetInitialPosition)
      if ((*i)->getInitialTilePosition()==position)
        passes=true;
    if (!passes)
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::inRadius(double radius,BWAPI::Position position) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if ((*i)->getDistance(position)<=radius)
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::inRegion(BWTA::Region* region) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (BWTA::getRegion((*i)->getTilePosition())==region)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::onlyNearestChokepoint(BWTA::Chokepoint* choke) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (BWTA::getNearestChokepoint((*i)->getTilePosition())==choke)
      result.insert(*i);
  }
  return result;
}

UnitGroup UnitGroup::onlyNearestBaseLocation(BWTA::BaseLocation* location) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (BWTA::getNearestBaseLocation((*i)->getTilePosition())==location)
      result.insert(*i);
  }
  return result;
}
UnitGroup UnitGroup::onlyNearestUnwalkablePolygon(BWTA::Polygon* polygon) const
{
  UnitGroup result;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    if (BWTA::getNearestUnwalkablePolygon((*i)->getTilePosition())==polygon)
      result.insert(*i);
  }
  return result;
}

UnitGroup& UnitGroup::operator+=(const UnitGroup& other)
{
  for(set<Unit*>::const_iterator i=other.begin();i!=other.end();i++)
    this->insert(*i);
  return *this;
}
UnitGroup& UnitGroup::operator*=(const UnitGroup& other)
{
  set<Unit*>::iterator i2;
  for(set<Unit*>::iterator i=this->begin();i!=this->end();i=i2)
  {
    i2=i;
    i2++;
    if (!other.contains(*i))
      this->erase(*i);
  }
  return *this;
}
UnitGroup& UnitGroup::operator^=(const UnitGroup& other)
{
  UnitGroup result=*this;
  for(set<Unit*>::const_iterator i=other.begin();i!=other.end();i++)
  {
    if (this->contains(*i))
      this->erase(*i);
    else
      this->insert(*i);
  }
  return *this;
}
UnitGroup& UnitGroup::operator-=(const UnitGroup& other)
{
  for(set<Unit*>::const_iterator i=other.begin();i!=other.end();i++)
    this->erase(*i);
  return *this;
}

BWAPI::Unit* UnitGroup::getNearest(BWAPI::Position position) const
{
  if (this->empty()) return NULL;
  set<Unit*>::const_iterator i=this->begin();
  Unit* result=*i;
  double d=(*i)->getDistance(position);
  i++;
  for(;i!=this->end();i++)
  {
    double d2=(*i)->getDistance(position);
    if (d2<d)
    {
      d=d2;
      result=*i;
    }
  }
  return result;
}

bool UnitGroup::contains(BWAPI::Unit* u) const
{
  return this->find(u)!=this->end();
}

Position UnitGroup::getCenter() const
{
  if (this->empty())
    return Positions::None;
  if (this->size()==1)
    return ((*this->begin())->getPosition());
  int count=0;
  double x=0;
  double y=0;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    Position p((*i)->getPosition());
    if (p!=Positions::None && p!=Positions::Unknown)
    {
      count++;
      x+=p.x();
      y+=p.y();
    }
  }
  if (count==0)
  {
    return Positions::None;
  }
  return Position((int)(x/count),(int)(y/count));
}

bool UnitGroup::attack(Position position) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->attack(position);
  }
  return retval;
}
bool UnitGroup::attackUnit(Unit* target) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->attack(target);
  }
  return retval;
}
bool UnitGroup::rightClick(Position position) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->rightClick(position);
  }
  return retval;
}
bool UnitGroup::rightClick(Unit* target) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->rightClick(target);
  }
  return retval;
}
bool UnitGroup::train(UnitType type) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->train(type);
  }
  return retval;
}
bool UnitGroup::build(TilePosition position, UnitType type) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->build(position,type);
  }
  return retval;
}
bool UnitGroup::buildAddon(UnitType type) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->buildAddon(type);
  }
  return retval;
}
bool UnitGroup::research(TechType tech) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->research(tech);
  }
  return retval;
}
bool UnitGroup::upgrade(UpgradeType upgrade) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->upgrade(upgrade);
  }
  return retval;
}
bool UnitGroup::stop() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->stop();
  }
  return retval;
}
bool UnitGroup::holdPosition() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->holdPosition();
  }
  return retval;
}
bool UnitGroup::patrol(Position position) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->patrol(position);
  }
  return retval;
}
bool UnitGroup::follow(Unit* target) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->follow(target);
  }
  return retval;
}
bool UnitGroup::setRallyPoint(Position target) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->setRallyPoint(target);
  }
  return retval;
}
bool UnitGroup::setRallyPoint(Unit* target) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->setRallyPoint(target);
  }
  return retval;
}
bool UnitGroup::repair(Unit* target) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->repair(target);
  }
  return retval;
}
bool UnitGroup::morph(UnitType type) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->morph(type);
  }
  return retval;
}
bool UnitGroup::burrow() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->burrow();
  }
  return retval;
}
bool UnitGroup::unburrow() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->unburrow();
  }
  return retval;
}
bool UnitGroup::siege() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->siege();
  }
  return retval;
}
bool UnitGroup::unsiege() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->unsiege();
  }
  return retval;
}
bool UnitGroup::cloak() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->cloak();
  }
  return retval;
}
bool UnitGroup::decloak() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->decloak();
  }
  return retval;
}
bool UnitGroup::lift() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->lift();
  }
  return retval;
}
bool UnitGroup::land(TilePosition position) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->land(position);
  }
  return retval;
}
bool UnitGroup::load(Unit* target) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->load(target);
  }
  return retval;
}
bool UnitGroup::unload(Unit* target) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->unload(target);
  }
  return retval;
}
bool UnitGroup::unloadAll() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->unloadAll();
  }
  return retval;
}
bool UnitGroup::unloadAll(Position position) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->unloadAll(position);
  }
  return retval;
}
bool UnitGroup::cancelConstruction() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->cancelConstruction();
  }
  return retval;
}
bool UnitGroup::haltConstruction() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->haltConstruction();
  }
  return retval;
}
bool UnitGroup::cancelMorph() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->cancelMorph();
  }
  return retval;
}
bool UnitGroup::cancelTrain() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->cancelTrain();
  }
  return retval;
}
bool UnitGroup::cancelTrain(int slot) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->cancelTrain(slot);
  }
  return retval;
}
bool UnitGroup::cancelAddon() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->cancelAddon();
  }
  return retval;
}
bool UnitGroup::cancelResearch() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->cancelResearch();
  }
  return retval;
}
bool UnitGroup::cancelUpgrade() const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->cancelUpgrade();
  }
  return retval;
}
bool UnitGroup::useTech(TechType tech) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->useTech(tech);
  }
  return retval;
}
bool UnitGroup::useTech(TechType tech, Position position) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->useTech(tech,position);
  }
  return retval;
}
bool UnitGroup::useTech(TechType tech, Unit* target) const
{
  bool retval=true;
  for(set<Unit*>::const_iterator i=this->begin();i!=this->end();i++)
  {
    retval = retval && (*i)->useTech(tech,target);
  }
  return retval;
}