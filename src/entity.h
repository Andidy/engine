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

	// Constructors and Assignment Operators ==================================

	cRenderable() = default;

	cRenderable(bool should_render, vec3 offset, vec3 scale, vec3 rot_axis,
		float rot_angle, AssetHandle h_model)
		: should_render(should_render), offset(offset), scale(scale),
		rot_axis(rot_axis), rot_angle(rot_angle), h_model(h_model) {}

	// Copy Ctor
	cRenderable(const cRenderable& source) {
		should_render = source.should_render;
		offset = source.offset;
		scale = source.scale;
		rot_axis = source.rot_axis;
		rot_angle = source.rot_angle;

		h_model.handle = source.h_model.handle;
		h_model.type = source.h_model.type;
		h_model.name = h_model.name;
	}

	// Copy op=
	cRenderable& operator= (const cRenderable& source) {
		assert(this != &source);
		
		should_render = source.should_render;
		offset = source.offset;
		scale = source.scale;
		rot_axis = source.rot_axis;
		rot_angle = source.rot_angle;

		h_model.handle = source.h_model.handle;
		h_model.type = source.h_model.type;
		h_model.name = h_model.name;
		
		return *this;
	}

	// Move Ctor
	cRenderable(cRenderable&& source) {
		should_render = source.should_render;
		offset = source.offset;
		scale = source.scale;
		rot_axis = source.rot_axis;
		rot_angle = source.rot_angle;

		h_model.handle = source.h_model.handle;
		h_model.type = source.h_model.type;
		h_model.name = h_model.name;

		source.should_render = false;
		source.offset = Vec3(0.0f, 0.0f, 0.0f);
		source.scale = Vec3(0.0f, 0.0f, 0.0f);
		source.rot_axis = Vec3(0.0f, 0.0f, 0.0f);
		source.rot_angle = 0.0f;

		source.h_model.handle = -1;
		source.h_model.type = AssetType::UNKNOWN;
		source.h_model.name = "";
	}

	// Move op=
	cRenderable& operator= (cRenderable&& source) {
		assert(this != &source);

		should_render = source.should_render;
		offset = source.offset;
		scale = source.scale;
		rot_axis = source.rot_axis;
		rot_angle = source.rot_angle;

		h_model.handle = source.h_model.handle;
		h_model.type = source.h_model.type;
		h_model.name = h_model.name;

		source.should_render = false;
		source.offset = Vec3(0.0f, 0.0f, 0.0f);
		source.scale = Vec3(0.0f, 0.0f, 0.0f);
		source.rot_axis = Vec3(0.0f, 0.0f, 0.0f);
		source.rot_angle = 0.0f;

		source.h_model.handle = -1;
		source.h_model.type = AssetType::UNKNOWN;
		source.h_model.name = "";

		return *this;
	}
};

/*
	The Transform component is responsible for representing the entity's position
	in the game world and associated information like orientation.
*/
struct cTransform {
	vec2 game_pos; // position in the game world of the entity

	// Constructors and Assignment Operators ==================================

	cTransform() = default;

	cTransform(vec2 game_pos)
		: game_pos(game_pos) {}

	// Copy Ctor
	cTransform(const cTransform& source) {
		game_pos = source.game_pos;
	}

	// Copy op=
	cTransform& operator= (const cTransform& source) {
		assert(this != &source);
		
		game_pos = source.game_pos;
		
		return *this;
	}

	// Move Ctor
	cTransform(cTransform&& source) {
		game_pos = source.game_pos;
		source.game_pos = Vec2(0.0f, 0.0f);
	}

	// Move op=
	cTransform& operator= (cTransform&& source) {
		assert(this != &source);
		
		game_pos = source.game_pos;
		source.game_pos = Vec2(0.0f, 0.0f);
		
		return *this;
	}
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
	int32_t inventory;

	// the default constructor creates a dead entity with no components. it
	// should be initialized with one of the InitXXXX functions further down
	Entity() : id(-1), is_active(false), transform(-1), renderable(-1), unit(-1),
		item(-1), food(-1), inventory(-1) {}

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

	void InitUnit(int _id, bool _is_active, int _transform, int _renderable, int _unit, int _inventory) {
		id = _id;
		is_active = _is_active;
		transform = _transform;
		renderable = _renderable;
		unit = _unit;
		inventory = _inventory;
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

typedef Entity EntityBlueprint;