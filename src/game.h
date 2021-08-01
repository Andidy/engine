#ifndef GAME_H
#define GAME_H

#include "universal.h"

#include "../libs/simplex.h"

struct Viewport {
	vec2 pos;
	vec2 size;
	vec2 depth; // x is min, y is max
};

struct Camera {
	vec3 pos;
	vec3 dir;
	vec3 up;

	f32 yaw, pitch;

	mat4 proj;
	mat4 view;
	mat4 inv_view;

	Viewport viewport;
};

struct Entity {
	vec3 renderPos;
	vec3 renderScale;
	vec3 renderRotAxis;
	f32 renderRotAngle;
};

struct GameState {
	i32 numEntities;
	Entity blackGuyHead;
	Entity blackGuyHead2;

	Entity bunnyTest;
	Entity bunnyTest2;

	Entity cubes[7];
	Entity quad;

	Camera mainCamera;

	PermanentResourceAllocator resourceAllocator;
};

void InitGameState(Memory* gameMemory, vec2 windowDimensions);
void GameUpdate(Memory* gameMemory, Input* gameInput, f32 dt, char* gameDebugText);

#endif
