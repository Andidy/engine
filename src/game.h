#ifndef GAME_H
#define GAME_H

#include "universal.h"

#include "../libs/simplex.h"

struct Camera {
	vec3 pos;
	vec3 target;
	vec3 up;

	Quaternion orientation;

	mat4 proj;
	mat4 view;
};

struct Entity {
	vec3 renderPos;
	vec3 renderScale;
};

struct GameMap {
	Entity ent;
	i32 mapWidth;
	i32 mapHeight;
	i32* elevationMap;
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
