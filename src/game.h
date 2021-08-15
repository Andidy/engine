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
	vec3 render_pos;
	vec3 render_scale;
	vec3 render_rot_axis;
	f32 render_rot_angle;
};

struct GameState {
	i32 num_entities;
	int blackGuyHead;
	int blackGuyHead2;

	int bunnyTest;
	int bunnyTest2;

	int cubes[7];
	int quad;

	int MAX_ENTITIES;
	Entity* entities;

	Camera main_camera;

	int picked_object;
	vec3 picking_dir;
	float picking_dist;

	PermanentResourceAllocator resource_allocator;
};

void InitGameState(Memory* game_memory, vec2 window_dimensions);
void GameUpdate(Memory* game_memory, Input* game_input, f32 dt, char* game_debug_text);
void UpdateCamera(Memory* game_memory, float x, float y);

#endif
