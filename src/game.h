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

enum TileResource {
	NONE,
	GRAINS,
	FRUITS,
	VEGETABLES,
	SPICES,
	FIBERPLANTS,
	SMALL_GAME,
	LARGE_GAME,
	SURFACE_ORE,
	DEEP_ORE
};

struct Tile {
	f32 elevation;
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
	GameMap gameMap;
	Camera mainCamera;

	PermanentResourceAllocator resourceAllocator;
};

void InitGameState(Memory* gameMemory, vec2 windowDimensions);
void GameUpdate(Memory* gameMemory, Input* gameInput, f32 dt);

#endif
