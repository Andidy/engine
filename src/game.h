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

	// A blueprint is an entity that has most of its data and components setup in
	// such a way so that when a new entity is to be created or spawned the game
	// can use the blueprint to simplify entity creation. For example, there will
	// be a blueprint for pine trees and it will have the components that pine
	// trees use already set up. Or a nomad unit, or a knight unit, or a loot
	// pile. The idea is that most of the data fields will already be filled out
	// and the creation function will only need to modify a handful of fields to
	// match the spawning context and current game state.
	std::vector<EntityBlueprint> blueprints;
	std::vector<cTransform> blueprint_transforms;
	std::vector<cRenderable> blueprint_renderables;
	std::vector<cUnit> blueprint_units;
	std::vector<cItem> blueprint_items;
	std::vector<cFood> blueprint_foods;
	std::vector<cInventory> blueprint_inventories;

	// The actual entities and components used by the game.
	std::vector<Entity> entities;
	std::vector<cTransform> c_transforms;
	std::vector<cRenderable> c_renderables;
	std::vector<cUnit> c_units;
	std::vector<cItem> c_items;
	std::vector<cFood> c_foods;
	std::vector<cInventory> c_inventories;

	Camera main_camera;

	int crosshair_entity;
	int arrow_head_entity;
	int arrow_shaft_entity;
	int arrow_butt_entity;
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
