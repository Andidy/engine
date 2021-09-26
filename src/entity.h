#pragma once

#include "universal.h"

/*
	The Renderable component contains information that allows the entity to be
	rendered by the Rendering system of the engine. It's purpose is focused on
	rendering and not gameplay or game world representation.
*/
struct cRenderable {
	bool should_render; // should this renderable be rendered this frame
	vec3 offset; // an offset of the model from the game world position
	vec3 scale; // the scaling of the model
	vec3 rot_axis; // the axis about which rotation is defined
	f32 rot_angle; // the angle of rotation
	AssetHandle h_model; // a handle to the model the renderable will be drawn as

	cRenderable(bool should_render, vec3 offset, vec3 scale, vec3 rot_axis,
		float rot_angle, AssetHandle h_model)
		: should_render(should_render), offset(offset), scale(scale),
		rot_axis(rot_axis), rot_angle(rot_angle), h_model(h_model) {}
};

/*
	The Transform component is responsible for representing the entity's position
	in the game world and associated information like orientation.
*/
struct cTransform {
	vec2 game_pos; // position in the game world of the entity

	cTransform(vec2 game_pos)
		: game_pos(game_pos) {}
};

/*
	Entity is a form of grouping for components that represents some element of
	the game world. It can define things such as: patches of grass, a sword,
	a nomad unit, a knight unit, a building, a trigger region for a quest, etc.
*/
struct Entity {
	int32_t id; // a unique identifier that the entity can be looked up by
	bool is_active; // is_active states whether this entity should be updated

	// the following are slots to attach created components to this entity
	// they are indexes into the GameState's c_xxxxx arrays
	int32_t transform;
	int32_t renderable;

	int32_t unit;
	int32_t item;
	int32_t food;

	// the default constructor creates a dead entity with no components. it
	// should be initialized with one of the InitXXXX functions further down
	Entity() : id(-1), is_active(false), transform(-1), renderable(-1), unit(-1),
		item(-1), food(-1) {}

	// The Init functions are responsible for attaching components to an entity
	// to create a type of game entity which has a purpose in the game world
	// For example: a unit which the player or an AI can control, an item like
	// a pile of coins which a unit might want to pick up, or a patch of grass
	// or other foliage
	void InitGeneric(int _id, bool _is_active, int _transform, int _renderable) {
		id = _id;
		is_active = _is_active;
		transform = _transform;
		renderable = _renderable;
	}

	void InitUnit(int _id, bool _is_active, int _transform, int _renderable, int _unit) {
		id = _id;
		is_active = _is_active;
		transform = _transform;
		renderable = _renderable;
		unit = _unit;
	}

	void InitItem(int _id, bool _is_active, int _transform, int _renderable, int _item) {
		id = _id;
		is_active = _is_active;
		transform = _transform;
		renderable = _renderable;
		item = _item;
	}

	void InitFood(int _id, bool _is_active, int _transform, int _renderable, int _item, int _food) {
		id = _id;
		is_active = _is_active;
		transform = _transform;
		renderable = _renderable;
		item = _item;
		food = _food;
	}
};

