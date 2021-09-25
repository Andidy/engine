#pragma once

#include <stdint.h>
#include <string>

struct Item {
	int32_t type; // food, weapon, loot
	int32_t amount; // how many of the item is in this "stack"
	int32_t value; // the monetary value of the item
};

struct FoodItem : Item {
	int32_t nourishment; // how much nourishment the food provides
};