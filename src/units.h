#pragma once

#include "universal.h"
#include <string>
#include <stdint.h>

struct Unit : Entity {
	bool waypoint_active; // should the movement waypoint disable and is it active
	vec2 waypoint_pos; // where the movement waypoint is

	std::vector<Item> inventory; // the unit's inventory
};