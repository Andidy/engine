#pragma once

#include "universal.h"

struct cRenderable {
	bool should_render;
	vec3 offset;
	vec3 scale;
	vec3 rot_axis;
	f32 rot_angle;
	AssetHandle h_model;

	cRenderable(bool should_render, vec3 offset, vec3 scale, vec3 rot_axis,
		float rot_angle, AssetHandle h_model)
		: should_render(should_render), offset(offset), scale(scale),
		rot_axis(rot_axis), rot_angle(rot_angle), h_model(h_model) {}
};

struct cTransform {
	vec2 game_pos; // position in the game world of the entity

	cTransform(vec2 game_pos)
		: game_pos(game_pos) {}
};

struct Entity {
	int32_t id;
	bool is_active;

	int32_t transform;
	int32_t renderable;

	int32_t unit;
	int32_t item;
	int32_t food;

	Entity() : id(-1), is_active(false), transform(-1), renderable(-1), unit(-1),
		item(-1), food(-1) {}

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

