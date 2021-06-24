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

struct TerrainMap {
	Entity ent;
	int32_t width;
	int32_t height;
	f32* elevation;
};

struct Location {
	float range = 10.0f;
	Entity ent;
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
	
	static const int numLocations = 1024;
	Location* locations;

	Camera mainCamera;

	PermanentResourceAllocator resourceAllocator;
};

void InitGameState(Memory* gameMemory, vec2 windowDimensions);
void GameUpdate(Memory* gameMemory, Input* gameInput, f32 dt, char* gameDebugText);

#endif
