#pragma once

#include "universal.h"

struct cItem {
	int32_t type; // food, weapon, loot
	int32_t amount; // how many of the item is in this "stack"

	cItem(int type, int amount)
		: type(type), amount(amount) {}
};

struct cFood {
	int32_t nourishment; // how much nourishment the food provides

	cFood(int nourishment)
		: nourishment(nourishment) {}
};