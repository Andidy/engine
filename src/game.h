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

#include "items.h"
#include "units.h"
#include "entity.h"

struct GameState {
	int64_t game_tick;
	float ticks_per_second;
	float tick_accumulator;
	bool game_ticked;

	// put "templates" here

	std::vector<Entity> entities;
	std::vector<cTransform> c_transforms;
	std::vector<cRenderable> c_renderables;
	std::vector<cUnit> c_units;
	std::vector<cItem> c_items;
	std::vector<cFood> c_foods;

	Camera main_camera;

	int crosshair_entity;
	int selected_entity;

	int picked_object;
	vec3 picking_dir;
	float picking_dist;
};

void LoadGameAssets(GameState* gs, AssetHandle* asset_handles);
void InitGameState(GameState* gs, vec2 window_dimensions, AssetHandle* asset_handles);
void GameUpdate(GameState* gs, Input* gi, f32 dt, char* game_debug_text);
void SetCameraViewportAndProjMat(Camera* camera, float x, float y);

#endif
