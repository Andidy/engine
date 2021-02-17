#ifndef GAME_H
#define GAME_H

#include "universal.h"

struct Camera {
	vec3 pos;
	vec3 target;
	vec3 up;

	mat4 proj;
	mat4 view;
};

struct GameState {
	ivec2 position;
	Camera mainCamera;
};

void InitGameState(Memory* gameMemory, vec2 windowDimensions);
void GameUpdate(Memory* gameMemory, Input* gameInput);

#endif
