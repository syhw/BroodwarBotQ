#ifndef ENUMS_NAME_TABLES
#define ENUMS_NAME_TABLES

#include <string>
#include <iostream>
#include <BWAPI.h>

/// Copyright Gabriel Synnaeve 2011
/// This code is under 3-clauses (new) BSD License

#define TERRAN_X_UNITS \
    X(Terran_Marine, (const char*) "Terran_Marine") \
    X(Terran_Ghost,  (const char*) "Terran_Ghost") \
    X(Terran_Vulture,  (const char*) "Terran_Vulture") \
    X(Terran_Goliath,  (const char*) "Terran_Goliath") \
    X(Terran_Siege,  (const char*) "Terran_Siege_Tank") \
    X(Terran_SCV,  (const char*) "Terran_SCV") \
    X(Terran_Wraith,  (const char*) "Terran_Wraith") \
    X(Terran_Science_Vessel,  (const char*) "Terran_Science_Vessel") \
    X(Terran_Dropship,  (const char*) "Terran_Dropship") \
    X(Terran_Battlecruiser,  (const char*) "Terran_Battlecruiser") \
    X(Terran_Vulture_Spider_Mine,  (const char*) "Terran_Vulture_Spider_Mine") \
    X(Terran_Nuclear_Missile,  (const char*) "Terran_Nuclear_Missile") \
    X(Terran_Siege_Tank_Siege_Mode,  (const char*) "Terran_Siege_Tank_Siege_Mode") \
    X(Terran_Firebat,  (const char*) "Terran_Firebat") \
    X(Terran_Medic,  (const char*) "Terran_Medic") \
    X(Terran_Valkyrie,  (const char*) "Terran_Valkyrie")

#define X(a, b) a,
enum Terran_Units
{
    TERRAN_X_UNITS
};
#undef X

#define X(a, b) b,
const char* terran_units_name[] = 
{
    TERRAN_X_UNITS
};
#undef X

#define NB_TERRAN_UNITS 16

#define TERRAN_X_BUILDINGS \
    X(Terran_Command_Center, (const char*) "Terran_Command_Center") \
    X(Terran_Command_Center2, (const char*) "Terran_Command_Center2") \
    X(Terran_Command_Center3, (const char*) "Terran_Command_Center3") \
    X(Terran_ComSat, (const char*) "Terran_ComSat") \
    X(Terran_Nuclear_Silo, (const char*) "Terran_Nuclear_Silo") \
    X(Terran_Supply_Depot, (const char*) "Terran_Supply_Depot") \
    X(Terran_Supply_Depot2, (const char*) "Terran_Supply_Depot2") \
    X(Terran_Supply_Depot3, (const char*) "Terran_Supply_Depot3") \
    X(Terran_Refinery, (const char*) "Terran_Refinery") \
    X(Terran_Refinery2, (const char*) "Terran_Refinery2") \
    X(Terran_Barracks, (const char*) "Terran_Barracks") \
    X(Terran_Barracks2, (const char*) "Terran_Barracks2") \
    X(Terran_Barracks3, (const char*) "Terran_Barracks3") \
    X(Terran_Barracks4, (const char*) "Terran_Barracks4") \
    X(Terran_Academy, (const char*) "Terran_Academy") \
    X(Terran_Factory, (const char*) "Terran_Factory") \
    X(Terran_Starport, (const char*) "Terran_Starport") \
    X(Terran_Control_Tower, (const char*) "Terran_Control_Tower") \
    X(Terran_Science_Facility, (const char*) "Terran_Science_Facility") \
    X(Terran_Covert_Ops, (const char*) "Terran_Covert_Ops") \
    X(Terran_Physics_Lab, (const char*) "Terran_Physics_Lab") \
    X(Terran_Machine_Shop, (const char*) "Terran_Machine_Shop") \
    X(Terran_Engineering_Bay, (const char*) "Terran_Engineering_Bay") \
    X(Terran_Armory, (const char*) "Terran_Armory") \
    X(Terran_Missile_Turret, (const char*) "Terran_Missile_Turret") \
    X(Terran_Bunker, (const char*) "Terran_Bunker")

#define X(a, b) a,
enum Terran_Buildings
{
    TERRAN_X_BUILDINGS
};
#undef X

#define X(a, b) b,
const char* terran_buildings_name[] = 
{
    TERRAN_X_BUILDINGS
};
#undef X

#define NB_TERRAN_BUILDINGS 26

#define PROTOSS_X_UNITS \
    X(Protoss_Corsair, (const char*) "Protoss_Corsair") \
    X(Protoss_Dark_Templar, (const char*) "Protoss_Dark_Templar") \
    X(Protoss_Dark_Archon, (const char*) "Protoss_Dark_Archon") \
    X(Protoss_Probe, (const char*) "Protoss_Probe") \
    X(Protoss_Zealot, (const char*) "Protoss_Zealot") \
    X(Protoss_Dragoon, (const char*) "Protoss_Dragoon") \
    X(Protoss_High_Templar, (const char*) "Protoss_High_Templar") \
    X(Protoss_Archon, (const char*) "Protoss_Archon") \
    X(Protoss_Shuttle, (const char*) "Protoss_Shuttle") \
    X(Protoss_Scout, (const char*) "Protoss_Scout") \
    X(Protoss_Arbiter, (const char*) "Protoss_Arbiter") \
    X(Protoss_Carrier, (const char*) "Protoss_Carrier") \
    X(Protoss_Interceptor, (const char*) "Protoss_Interceptor") \
    X(Protoss_Reaver, (const char*) "Protoss_Reaver") \
    X(Protoss_Observer, (const char*) "Protoss_Observer") \
    X(Protoss_Scarab, (const char*) "Protoss_Scarab")

#define X(a, b) a,
enum Protoss_Units
{
    PROTOSS_X_UNITS
};
#undef X

#define X(a, b) b,
const char* protoss_units_name[] =
{
    PROTOSS_X_UNITS
};
#undef X

#define NB_PROTOSS_UNITS 16

#define PROTOSS_X_BUILDINGS \
    X(Protoss_Nexus, (const char*) "Protoss_Nexus") \
    X(Protoss_Nexus2, (const char*) "Protoss_Nexus2") \
    X(Protoss_Nexus3, (const char*) "Protoss_Nexus3") \
    X(Protoss_Robotics_Facility, (const char*) "Protoss_Robotics_Facility") \
    X(Protoss_Pylon, (const char*) "Protoss_Pylon") \
    X(Protoss_Pylon2, (const char*) "Protoss_Pylon2") \
    X(Protoss_Pylon3, (const char*) "Protoss_Pylon3") \
    X(Protoss_Assimilator, (const char*) "Protoss_Assimilator") \
    X(Protoss_Assimilator2, (const char*) "Protoss_Assimilator2") \
    X(Protoss_Observatory, (const char*) "Protoss_Observatory") \
    X(Protoss_Gateway, (const char*) "Protoss_Gateway") \
    X(Protoss_Gateway2, (const char*) "Protoss_Gateway2") \
    X(Protoss_Gateway3, (const char*) "Protoss_Gateway3") \
    X(Protoss_Gateway4, (const char*) "Protoss_Gateway4") \
    X(Protoss_Photon_Cannon, (const char*) "Protoss_Photon_Cannon") \
    X(Protoss_Citadel_of_Adun, (const char*) "Protoss_Citadel_of_Adun") \
    X(Protoss_Cybernetics_Core, (const char*) "Protoss_Cybernetics_Core") \
    X(Protoss_Templar_Archives, (const char*) "Protoss_Templar_Archives") \
    X(Protoss_Forge, (const char*) "Protoss_Forge") \
    X(Protoss_Stargate, (const char*) "Protoss_Stargate") \
    X(Protoss_Fleet_Beacon, (const char*) "Protoss_Fleet_Beacon") \
    X(Protoss_Arbiter_Tribunal, (const char*) "Protoss_Arbiter_Tribunal") \
    X(Protoss_Robotics_Support_Bay, (const char*) "Protoss_Robotics_Support_Bay") \
    X(Protoss_Shield_Battery, (const char*) "Protoss_Shield_Battery")

#define X(a, b) a,
enum Protoss_Buildings
{
    PROTOSS_X_BUILDINGS
};
#undef X

#define X(a, b) b,
const char* protoss_buildings_name[] =
{
    PROTOSS_X_BUILDINGS
};
#undef X

#define NB_PROTOSS_BUILDINGS 24

#define ZERG_X_UNITS \
    X(Zerg_Larva, (const char*) "Zerg_Larva") \
    X(Zerg_Egg, (const char*) "Zerg_Egg") \
    X(Zerg_Zergling, (const char*) "Zerg_Zergling") \
    X(Zerg_Hydralisk, (const char*) "Zerg_Hydralisk") \
    X(Zerg_Ultralisk, (const char*) "Zerg_Ultralisk") \
    X(Zerg_Broodling, (const char*) "Zerg_Broodling") \
    X(Zerg_Drone, (const char*) "Zerg_Drone") \
    X(Zerg_Mutalisk, (const char*) "Zerg_Mutalisk") \
    X(Zerg_Guardian, (const char*) "Zerg_Guardian") \
    X(Zerg_Queen, (const char*) "Zerg_Queen") \
    X(Zerg_Defiler, (const char*) "Zerg_Defiler") \
    X(Zerg_Scourge, (const char*) "Zerg_Scourge") \
    X(Zerg_Infested_Terran, (const char*) "Zerg_Infested_Terran") \
    X(Zerg_Cocoon, (const char*) "Zerg_Cocoon") \
    X(Zerg_Devourer, (const char*) "Zerg_Devourer") \
    X(Zerg_Lurker_Egg, (const char*) "Zerg_Lurker_Egg") \
    X(Zerg_Lurker, (const char*) "Zerg_Lurker")

#define X(a, b) a,
enum Zerg_Units
{
    ZERG_X_UNITS
};
#undef X

#define X(a, b) b,
const char* zerg_units_name[] =
{
    ZERG_X_UNITS
};
#undef X

#define NB_ZERG_UNITS 17

#define ZERG_X_BUILDINGS \
    X(Zerg_Hatchery, (const char*) "Zerg_Hatchery") \
    X(Zerg_Hatchery2, (const char*) "Zerg_Hatchery2") \
    X(Zerg_Hatchery3, (const char*) "Zerg_Hatchery3") \
    X(Zerg_Hatchery4, (const char*) "Zerg_Hatchery4") \
    X(Zerg_Lair, (const char*) "Zerg_Lair") \
    X(Zerg_Hive, (const char*) "Zerg_Hive") \
    X(Zerg_Nydus_Canal, (const char*) "Zerg_Nydus_Canal") \
    X(Zerg_Hydralisk_Den, (const char*) "Zerg_Hydralisk_Den") \
    X(Zerg_Defiler_Mound, (const char*) "Zerg_Defiler_Mound") \
    X(Zerg_Greater_Spire, (const char*) "Zerg_Greater_Spire") \
    X(Zerg_Queens_Nest, (const char*) "Zerg_Queens_Nest") \
    X(Zerg_Evolution_Chamber, (const char*) "Zerg_Evolution_Chamber") \
    X(Zerg_Ultralisk_Cavern, (const char*) "Zerg_Ultralisk_Cavern") \
    X(Zerg_Spire, (const char*) "Zerg_Spire") \
    X(Zerg_Spawning_Pool, (const char*) "Zerg_Spawning_Pool") \
    X(Zerg_Creep_Colony, (const char*) "Zerg_Creep_Colony") \
    X(Zerg_Spore_Colony, (const char*) "Zerg_Spore_Colony") \
    X(Zerg_Sunken_Colony, (const char*) "Zerg_Sunken_Colony") \
    X(Zerg_Extractor, (const char*) "Zerg_Extractor")\
    X(Zerg_Extractor2, (const char*) "Zerg_Extractor2")\
    \
    X(Zerg_Overlord, (const char*) "Zerg_Overlord") \
    X(Zerg_Overlord2, (const char*) "Zerg_Overlord2") \
    X(Zerg_Overlord3, (const char*) "Zerg_Overlord3") \

#define X(a, b) a,
enum Zerg_Buildings
{
    ZERG_X_BUILDINGS
};
#undef X

#define X(a, b) b,
const char* zerg_buildings_name[] =
{
    ZERG_X_BUILDINGS
};
#undef X

#define NB_ZERG_BUILDINGS 24

enum Spells
{
    Spell_Scanner_Sweep,
    Spell_Disruption_Web,
    Spell_Dark_Swarm
};

class Building
{
    friend std::ostream& operator <<(std::ostream& os, const Building& b);
    int _enumValue;
    int _tableSize;
    const char** _nameTable;
public:
    Building(Protoss_Buildings v)
        : _enumValue(v)
        , _nameTable(protoss_buildings_name)
        , _tableSize(NB_PROTOSS_BUILDINGS)
    {
    }
    Building(Terran_Buildings v)
        : _enumValue(v)
        , _nameTable(terran_buildings_name)
        , _tableSize(NB_TERRAN_BUILDINGS)
    {
    }
    Building(Zerg_Buildings v)
        : _enumValue(v)
        , _nameTable(zerg_buildings_name)
        , _tableSize(NB_ZERG_BUILDINGS)
    {
    }
    Building(const char* buildingName)
    {
        if (buildingName[0] == 'P')
        {
            _tableSize = NB_PROTOSS_BUILDINGS;
            _nameTable = protoss_buildings_name;
        } 
        else if (buildingName[0] == 'T')
        {
            _tableSize = NB_TERRAN_BUILDINGS;
            _nameTable = terran_buildings_name;
        } 
        else if (buildingName[0] == 'Z')
        {
            _tableSize = NB_ZERG_BUILDINGS;
            _nameTable = zerg_buildings_name;
        } 
        else
        {
			BWAPI::Broodwar->printf("ERROR: Building constructor failed to determine the race -> %s", buildingName);
        }
        for (int i = 0; i < _tableSize; i++)
        {
            if (!strcmp(buildingName, _nameTable[i]))
            {
                _enumValue = i;
                return;
            }
        }
		BWAPI::Broodwar->printf("ERROR: not found this building: %s", buildingName);
    }
    std::ostream& operator <<(std::ostream& os) const
    {
        if (_enumValue < _tableSize)
            os << std::string(_nameTable[_enumValue]);
        else
            os << "ERROR: _enumValue too big: " << _enumValue;
        return os;
    }
    int getEnumValue() const
    {
        return _enumValue;
    }
	const char** getName() const
	{
		return _nameTable;
	}
};
inline std::ostream& operator <<(std::ostream& os, const Building& b)
{
    if (b._enumValue < b._tableSize)
        os << std::string(b._nameTable[b._enumValue]);
    else
        os << "ERROR: _enumValue too big: " << b._enumValue;
    return os;
}

struct tree_node
{
    int value;
    std::map<int, tree_node*> children;
    void append(int v)
    {
        children.insert(std::make_pair<int, tree_node*>(v, new tree_node(v)));
    }
    void append(tree_node* tn)
    {
        children.insert(std::make_pair<int, tree_node*>(tn->value, tn));
    }
    tree_node(int v): value(v) {}
    tree_node(const char* p) : value(Protoss_Nexus) // root
    {
        if (p[0] == 'P')
        {
            this->append(Protoss_Nexus);
            this->append(Protoss_Nexus2);
            this->append(Protoss_Nexus3);
            this->append(Protoss_Pylon);
            this->append(Protoss_Pylon2);
            this->append(Protoss_Pylon3);
            this->append(Protoss_Assimilator);
            this->append(Protoss_Assimilator2);
            this->append(Protoss_Gateway);
            this->append(Protoss_Gateway2);
            this->append(Protoss_Gateway3);
            this->append(Protoss_Gateway4);
            this->append(Protoss_Forge);
            children[Protoss_Forge]->append(Protoss_Photon_Cannon);
            children[Protoss_Gateway]->append(Protoss_Shield_Battery);
            children[Protoss_Gateway]->append(Protoss_Cybernetics_Core);
            children[Protoss_Gateway]->children[Protoss_Cybernetics_Core]->
                append(Protoss_Stargate);
            children[Protoss_Gateway]->children[Protoss_Cybernetics_Core]->
                append(Protoss_Citadel_of_Adun);
            children[Protoss_Gateway]->children[Protoss_Cybernetics_Core]->
                append(Protoss_Robotics_Facility);
            children[Protoss_Gateway]->children[Protoss_Cybernetics_Core]->
                children[Protoss_Robotics_Facility]->
                append(Protoss_Robotics_Support_Bay);
            children[Protoss_Gateway]->children[Protoss_Cybernetics_Core]->
                children[Protoss_Robotics_Facility]->append(Protoss_Observatory);
            children[Protoss_Gateway]->children[Protoss_Cybernetics_Core]->
                children[Protoss_Citadel_of_Adun]->
                append(Protoss_Templar_Archives);
            children[Protoss_Gateway]->children[Protoss_Cybernetics_Core]->
                children[Protoss_Stargate]->append(Protoss_Fleet_Beacon);
            tree_node* tribunal = new tree_node(Protoss_Arbiter_Tribunal);
            children[Protoss_Gateway]->children[Protoss_Cybernetics_Core]->
                children[Protoss_Citadel_of_Adun]->
                children[Protoss_Templar_Archives]->append(tribunal);
            children[Protoss_Gateway]->children[Protoss_Cybernetics_Core]->
                children[Protoss_Stargate]->append(tribunal);
        }
        else if (p[0] == 'T')
        {
            this->append(Terran_Command_Center);
            this->append(Terran_Command_Center2);
            this->append(Terran_Command_Center3);
            this->append(Terran_ComSat);
            this->append(Terran_Nuclear_Silo);
            this->append(Terran_Supply_Depot);
            this->append(Terran_Supply_Depot2);
            this->append(Terran_Supply_Depot3);
            this->append(Terran_Refinery);
            this->append(Terran_Refinery2);
            this->append(Terran_Barracks);
            this->append(Terran_Barracks2);
            this->append(Terran_Barracks3);
            this->append(Terran_Barracks4);
            this->append(Terran_Engineering_Bay);
            children[Terran_Engineering_Bay]->append(Terran_Missile_Turret);
            children[Terran_Barracks]->append(Terran_Academy);
            children[Terran_Barracks]->append(Terran_Factory);
            children[Terran_Barracks]->append(Terran_Bunker);
            children[Terran_Barracks]->children[Terran_Factory]->
                append(Terran_Starport);
            children[Terran_Barracks]->children[Terran_Factory]->
                append(Terran_Armory);
            children[Terran_Barracks]->children[Terran_Factory]->
                append(Terran_Machine_Shop);
            children[Terran_Barracks]->children[Terran_Factory]->
                children[Terran_Starport]->append(Terran_Control_Tower);
            children[Terran_Barracks]->children[Terran_Factory]->
                children[Terran_Starport]->append(Terran_Science_Facility);
            children[Terran_Barracks]->children[Terran_Factory]->
            children[Terran_Starport]->children[Terran_Science_Facility]->
                append(Terran_Covert_Ops);
            children[Terran_Barracks]->children[Terran_Factory]->
            children[Terran_Starport]->children[Terran_Science_Facility]->
                append(Terran_Physics_Lab);
        }
        else if (p[0] == 'Z')
        {
            this->append(Zerg_Hatchery);
            this->append(Zerg_Hatchery2);
            this->append(Zerg_Hatchery3);
            this->append(Zerg_Hatchery4);
            this->append(Zerg_Evolution_Chamber);
            this->append(Zerg_Spawning_Pool);
            this->append(Zerg_Creep_Colony);
            this->append(Zerg_Extractor);
            this->append(Zerg_Extractor2);
            this->append(Zerg_Overlord);
            this->append(Zerg_Overlord2);
            this->append(Zerg_Overlord3);
            children[Zerg_Evolution_Chamber]->append(Zerg_Spore_Colony);
            children[Zerg_Spawning_Pool]->append(Zerg_Sunken_Colony);
            children[Zerg_Spawning_Pool]->append(Zerg_Hydralisk_Den);
            children[Zerg_Hatchery]->append(Zerg_Lair);
            children[Zerg_Spawning_Pool]->append(children[Zerg_Hatchery]->
                    children[Zerg_Lair]);
            children[Zerg_Hatchery]->children[Zerg_Lair]->
                append(Zerg_Spire);
            children[Zerg_Hatchery]->children[Zerg_Lair]->
                append(Zerg_Queens_Nest);
            children[Zerg_Hatchery]->children[Zerg_Lair]->append(Zerg_Hive);
            children[Zerg_Hatchery]->children[Zerg_Lair]->
                children[Zerg_Queens_Nest]->append(children[Zerg_Hatchery]->
                        children[Zerg_Lair]->children[Zerg_Hive]);
            children[Zerg_Hatchery]->children[Zerg_Lair]->children[Zerg_Hive]
                ->append(Zerg_Nydus_Canal);
            children[Zerg_Hatchery]->children[Zerg_Lair]->children[Zerg_Hive]
                ->append(Zerg_Defiler_Mound);
            tree_node* gspire = new tree_node(Zerg_Greater_Spire);
            children[Zerg_Hatchery]->children[Zerg_Lair]->children[Zerg_Hive]
                ->append(gspire);
            children[Zerg_Hatchery]->children[Zerg_Lair]->children[Zerg_Spire]
                ->append(gspire);
            children[Zerg_Hatchery]->children[Zerg_Lair]->children[Zerg_Hive]
                ->append(Zerg_Ultralisk_Cavern);
        }
    }
    ~tree_node()
    {
        for (std::map<int, tree_node*>::iterator it = children.begin();
                it != children.end(); ++it)
        {
            if (it->second != NULL)
                delete it->second;
        }
    }
};

#endif
