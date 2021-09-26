#pragma once

#include "universal.h"

struct cUnit {
	bool waypoint_active; // is the movement waypoint active
	vec2 waypoint_pos; // where the movement waypoint is

	cUnit(bool waypoint_active, vec2 waypoint_pos)
		: waypoint_active(waypoint_active), waypoint_pos(waypoint_pos) {}
};