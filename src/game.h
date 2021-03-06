#ifndef GAME_H
#define GAME_H

#include "universal.h"

#include "../libs/simplex.h"

struct Camera {
	vec3 pos;
	vec3 dir;
	vec3 up;

	f32 yaw, pitch;

	mat4 proj;
	mat4 view;
};

struct Entity {
	vec3 renderPos;
	vec3 renderScale;
};

enum class TileTerrain {
	WATER,
	FLATLAND,
	HILLS,
	LOWMOUNTAINS,
	HIGHMOUNTAINS
};

enum class TileFeatures {
	NONE,
	FOREST,
	MARSH,
	TUNDRA,
	GLACIER,
	DESERT,
	COAST,
	RIVER,
	OCEAN
};

enum class TileWeather {
	NONE,
	FLOODED,
	SNOW,
	DROUGHT
};

enum class TileResource {
	NONE,
	GRAINS, // wheat, rice, barley, etc. 
	FRUITS, // apples, grapes, oranges, etc.
	VEGETABLES, // lettuce, onions, tomatoes, roots, etc.
	SPICES, // pepper, cumin, paprika, etc.
	TEXTILES, // cotton, flax, hemp, dyes, etc.
	SMALL_GAME, // rabbits, rats, raccoons, beavers, etc.
	LARGE_GAME, // deer, horse, moose, etc.
	PREDATORS, // bears, wolves, big cats, etc.
	CLAY, // earth and clay used for building or pottery
	STONE, // for building stuff: limestone, sandstone, marble, etc.
	SURFACE_ORE, // both ores have the same types of resources, its just how they're extracted:
	DEEP_ORE // copper, tin, iron, silver, gold, gems, coal?, nickel?, 
};
 
struct Tile {
	f32 elevation;
	TileTerrain terrain;
	TileFeatures feature;
	TileWeather weather;
	TileResource resources[3];
};

struct GameMap {
	Entity ent;
	i32 mapWidth;
	i32 mapHeight;
	Tile* tiles;
};

struct GameState {
	Entity blackGuyHead;
	Entity deerTest;
	Entity bunnyTest;
	Entity treeTest;
	Entity cubes[7];

	GameMap gameMap;
	Camera mainCamera;

	PermanentResourceAllocator resourceAllocator;
};

void InitGameState(Memory* gameMemory, vec2 windowDimensions);
void GameUpdate(Memory* gameMemory, Input* gameInput, f32 dt);

#endif
