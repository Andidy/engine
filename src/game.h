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
	vec3 renderRotAxis;
	f32 renderRotAngle;
};

enum class TileNeighbor {
	NORTHWEST,
	NORTH,
	NORTHEAST,
	EAST,
	SOUTHEAST,
	SOUTH,
	SOUTHWEST,
	WEST
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
	bool wrapHorizontal;
	bool wrapVertical;
	Entity ent;
	int32_t mapWidth;
	int32_t mapHeight;
	Tile* tiles;
};

ivec2 GetNeighborPosition(GameMap* gameMap, int32_t x, int32_t y, TileNeighbor direction);
ivec2 GetNeighborPosition(GameMap* gameMap, int32_t tileIndex, TileNeighbor direction);
int32_t GetNeighborIndex(GameMap* gameMap, int32_t x, int32_t y, TileNeighbor direction);
int32_t GetNeighborIndex(GameMap* gameMap, int32_t tileIndex, TileNeighbor direction);

struct TerrainMap {
	Entity ent;
	int32_t width;
	int32_t height;
	f32* elevation;
};

struct GameState {
	i32 numEntities;
	Entity blackGuyHead;
	Entity deerTest;
	Entity bunnyTest;
	Entity treeTest;
	Entity cubes[7];
	Entity quad;
	Entity waterQuad;

	TerrainMap terrainMap;
	GameMap gameMap;
	Camera mainCamera;

	PermanentResourceAllocator resourceAllocator;
};

void InitGameState(Memory* gameMemory, vec2 windowDimensions);
void GameUpdate(Memory* gameMemory, Input* gameInput, f32 dt, char* gameDebugText);

#endif
