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
	std::string name;
	vec3 render_offset;
	vec3 render_scale;
	vec3 render_rot_axis;
	f32 render_rot_angle;
	AssetHandle h_model;

	vec2 game_pos;
	bool should_render;

	bool is_unit;
	// unit specific stuff
	bool waypoint_active;
	vec2 waypoint_pos;
};

struct GameState {
	int64_t game_tick;
	float ticks_per_second;
	float tick_accumulator;
	bool game_ticked;
	
	i32 num_entities;
	int MAX_ENTITIES;
	Entity* entities;

	Camera main_camera;

	int crosshair_entity;

	int selected_entity;

	int picked_object;
	vec3 picking_dir;
	float picking_dist;

	PermanentResourceAllocator resource_allocator;
};

void LoadGameAssets(GameState* gs, AssetHandle* asset_handles);
void InitGameState(Memory* game_memory, vec2 window_dimensions, AssetHandle* asset_handles);
void GameUpdate(Memory* game_memory, Input* game_input, f32 dt, char* game_debug_text);
void SetMainCameraViewportAndProjMat(Memory* game_memory, float x, float y);

#endif
