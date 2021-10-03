#pragma once

#include "universal.h"
#include "entity.h"

/*
	The item component is the primary marker of a class of gameplay elements
	which can be acquired and used by units and stored in a location like a
	building or structure.
*/
struct cItem {
	enum class Type {
		None = 0,
		Valuable = 1,
		Food = 2,
		Weapon = 3,
		Tool = 4,
		Wood = 5,
		Stone = 6
	};
	Type type;
	int32_t value;

	cItem(int _type, int _value) {
		type = (Type)_type;
		value = _value;
	}
};

/*
	The food component allows a unit entity to consume this entity to continue
	to live.
*/
struct cFood {
	int32_t nourishment; // how much nourishment the food provides

	cFood(int nourishment)
		: nourishment(nourishment) {}
};

/*
	ItemPile is a helper structure to allow the inventory component to store
	items and the quantity of those items together, because the quantity isn't
	an intrinsic value of an item.

	TODO: Perhaps the entity shouldn't be a full entity and instead just an index
	into the array of entities in GameState?
*/
struct ItemPile {
	Entity e;
	int32_t quantity;
};

/*
	The inventory component allows an entity to store references to other entities
	and the quantity of those entities. Ex: 10 Carrots, 1 Sword, and 128 coins.
*/
struct cInventory {
	std::vector<ItemPile> items;
};