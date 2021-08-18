#ifndef ASSET_LOADING_H
#define ASSET_LOADING_H

#include <string>
#include "universal.h"
/*
#include <vector>
#include <unordered_map>

#include"../libs/json11-master/json11.hpp"

#include "renderer.h"
*/
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

/*
struct TestEntity {
	vec3 render_pos;
	vec3 render_scale;
	vec3 render_rot_axis;
	f32 render_rot_angle;
	AssetHandle h_model;
};

AssetHandle ResolveMissingAsset(std::string str, std::unordered_map<std::string, AssetHandle>* asset_handles) {
	
	
	debug_ReadFileResult file = debug_ReadFile((char*)"test_assets/models.json");

	if (file.data != NULL && file.size >= 0) {
		std::string json_err_str;
		json11::Json json = json11::Json::parse((char*)file.data, json_err_str);

		int asset_index = 0;
		while (json[asset_index].is_object()) {
			json11::Json::object ja = json[asset_index].object_items();
			if (str.compare(ja["name"].string_value()) == 0) {

			}
		}
	}

	return { "blank", AssetType::UNKNOWN, 0 };
}

void LoadAssets() {
	debug_ReadFileResult file = debug_ReadFile((char*)"test_assets/entities.json");
	
	std::vector<TestEntity> entities;

	std::unordered_map<std::string, AssetHandle> map;

	if (file.data != NULL && file.size >= 0) {
		std::string json_err_str;
		json11::Json json = json11::Json::parse((char*)file.data, json_err_str);
	
		int entity_index = 0;
		while (json[entity_index].is_object()) {
			json11::Json::object je = json[entity_index].object_items();
			TestEntity e = {};
			
			auto pos = je["pos"].array_items();
			e.render_pos = Vec3(pos[0].number_value(), pos[1].number_value(), pos[2].number_value());
			
			auto scale = je["scale"].array_items();
			e.render_scale = Vec3(scale[0].number_value(), scale[1].number_value(), scale[2].number_value());
			
			auto rotation_axis = je["rotation_axis"].array_items();
			e.render_rot_axis = Vec3(rotation_axis[0].number_value(), rotation_axis[1].number_value(), rotation_axis[2].number_value());
			
			auto rotation_angle = je["rotation_angle"].number_value();
			e.render_rot_angle = (float)rotation_angle;

			auto asset_name = je["model"].string_value();
			auto search = map.find(asset_name);
			if (search != map.end()) {
				// if the asset has already been loaded just provide the existing handle
				e.h_model = search->second;
			}
			else {
				// if the asset hasn't been loaded, load the asset and return the new handle
				e.h_model = ResolveMissingAsset(asset_name, &map);
			}

			entities.push_back(e);
		}
	}
}
*/

#endif