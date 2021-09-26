#pragma once

#include "universal.h"

/*
	the unit component is the primary marker of the gameplay definition of a unit
	which is some living thing in the game world that can be controlled and
	interacted with by the player and AI's. Some examples of units are: an army,
	a trading caravan, a fleet of ships, and a herd of animals.
*/
struct cUnit {
	bool waypoint_active; // is the movement waypoint active
	vec2 waypoint_pos; // where the movement waypoint is

	cUnit(bool waypoint_active, vec2 waypoint_pos)
		: waypoint_active(waypoint_active), waypoint_pos(waypoint_pos) {}
};