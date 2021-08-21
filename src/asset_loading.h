#ifndef ASSET_LOADING_H
#define ASSET_LOADING_H

#include <string>

enum class AssetType {
	UNKNOWN,
	MODEL,
	MESH,
	TEXTURE,
	NUM_TYPES
};

struct AssetHandle {
	std::string name;
	AssetType type;
	int32_t handle;
};

#endif