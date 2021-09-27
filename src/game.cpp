#include "game.h"

struct MouseRayReturn {
	vec3 start;
	vec3 direction;
};

MouseRayReturn MouseRay(GameState* gs, Input* gi) {
	// calculate the start point and direction vector of the picking ray
	float m00 = gs->main_camera.proj.data[0][0];
	float m11 = gs->main_camera.proj.data[1][1];
	vec4 camera_space_vector = {
		(2.0f * ((f32)gi->mouse.x) / gs->main_camera.viewport.size.x - 1.0f) / m00,
		-(2.0f * ((f32)gi->mouse.y) / gs->main_camera.viewport.size.y - 1.0f) / m11,
		-1.0f,
		0.0f
	};

	vec4 dir = gs->main_camera.inv_view * camera_space_vector;
	vec3 direction_vector = Normalize({ dir.x, dir.y, dir.z });
	vec3 start_vector = {
		gs->main_camera.inv_view.data[0][3],
		gs->main_camera.inv_view.data[1][3],
		gs->main_camera.inv_view.data[2][3]
	};

	return { start_vector, direction_vector };
}

// The return type is the point on the plane intersected with, Vec3(INF, INF, INF) means the ray cast is parallel to the plane.
vec3 RayPlaneCollisionCheck(vec3 ray_start, vec3 ray_dir, vec3 plane_point, vec3 plane_normal) {
	vec3 result = { INFINITY, INFINITY, INFINITY };

	float d = Dot(plane_point, NegVec(plane_normal));
	float t = -(d + Dot(ray_start, plane_normal)) / Dot(ray_dir, plane_normal);
	if (t > 0.00001f) {
		return ray_start + (ray_dir * t);
	}
	else {
		return result;
	}
}

bool RayQuadCollisionCheck(vec3 ray_start, vec3 ray_dir, vec3 quad_pos, vec3 quad_normal) {
	vec3 plane_intersection_point = RayPlaneCollisionCheck(ray_start, ray_dir, quad_pos, quad_normal);
	float dist = fabsf(Distance(quad_pos, plane_intersection_point));
	return dist <= 0.5f;
}

void SetCameraViewportAndProjMat(Camera* camera, float width, float height) {
	camera->viewport.size.x = width;
	camera->viewport.size.y = height;
	camera->proj = PerspectiveMat(90.0f, width / height, 0.1f, 1000.0f);
}

bool PickupItem(GameState* gs, Entity* e, Entity* item) {
	if (item->item >= 0) {
		// e->coins += item->coins;

		gs->c_renderables[item->renderable].should_render = false;
		item->is_active = false;
		return true;
	}
	else {
		return false;
	}
}

void InitGameState(GameState* gs, vec2 window_dimensions, AssetHandle* asset_handles) {
	// Experimental asset loading

	LoadGameAssets(gs, asset_handles);

	// end Experimental asset loading

	gs->game_tick = 0;
	gs->ticks_per_second = 10.0f;

	gs->selected_entity = -1;

	gs->picked_object = -1;
	gs->picking_dir = ZeroVec();
	gs->picking_dist = 0.0f;

	gs->main_camera.pos = Vec3(-10.0f, 10.0f, 0.0f);
	gs->main_camera.up = Vec3(0.0, 1.0, 0.0f);

	gs->main_camera.pitch = 0.0f;
	gs->main_camera.yaw = -90.0f;

	vec3 dir = Vec3(
		cosf(DegToRad(gs->main_camera.yaw)) * cosf(DegToRad(gs->main_camera.pitch)),
		sinf(DegToRad(gs->main_camera.pitch)),
		sinf(DegToRad(gs->main_camera.yaw)) * cosf(DegToRad(gs->main_camera.pitch))
	);
	gs->main_camera.dir = Normalize(dir);

	gs->main_camera.view = LookAtMat(gs->main_camera.pos, gs->main_camera.pos + gs->main_camera.dir, gs->main_camera.up);
	gs->main_camera.inv_view = InverseLookAtMat(gs->main_camera.pos, gs->main_camera.pos + gs->main_camera.dir, gs->main_camera.up);
	gs->main_camera.proj = PerspectiveMat(90.0f, window_dimensions.x / window_dimensions.y, 0.1f, 1000.0f);

	Viewport viewport = {};
	viewport.pos = Vec2(0.0f, 0.0f);
	viewport.size = Vec2(window_dimensions.x, window_dimensions.y);
	viewport.depth = Vec2(0.0f, 1.0f);
	gs->main_camera.viewport = viewport;
}

void GameUpdate(GameState* gs, Input* gi, f32 dt, char* game_debug_text) {
	gs->tick_accumulator += dt;
	if (gs->tick_accumulator > (1.0f / gs->ticks_per_second)) {
		gs->tick_accumulator = 0.0f;
		gs->game_tick += 1;
		gs->game_ticked = true;
	}

	// ========================================================================
	// Camera Update
	Camera* camera = &gs->main_camera;
	{
		const f32 base_camera_speed = 10.0f;
		const f32 speed_multiplier = 50.0f;
		f32 camera_speed = base_camera_speed;

		if (keyDown(gi->keyboard.space)) {
			camera_speed *= speed_multiplier;
		}

		const f32 rotateSpeed = 36.0f;

		vec3 dir = Normalize(camera->dir);
		vec3 right = Normalize(Cross(camera->up, dir));

		if (keyDown(gi->keyboard.w)) {
			vec3 temp_dir = dir - UpVec() * (Dot(dir, UpVec()) / powf(dir.Len(), 2.0f));
			temp_dir = Normalize(temp_dir) * (camera_speed * dt);
			camera->pos += temp_dir;
		}
		else if (keyDown(gi->keyboard.s)) {
			vec3 temp_dir = dir - UpVec() * (Dot(dir, UpVec()) / powf(dir.Len(), 2.0f));
			temp_dir = Normalize(temp_dir) * (camera_speed * dt);
			camera->pos -= temp_dir;
		}
		else if (keyDown(gi->keyboard.up)) {
			dir *= (camera_speed * dt);
			gs->main_camera.pos += dir;
		}
		else if (keyDown(gi->keyboard.down)) {
			dir *= (camera_speed * dt);
			gs->main_camera.pos -= dir;
		}

		if (keyDown(gi->keyboard.t)) {
			camera->pos += UpVec() * (camera_speed * dt);
		}
		else if (keyDown(gi->keyboard.g)) {
			camera->pos -= UpVec() * camera_speed * dt;
		}

		if (keyDown(gi->keyboard.left) || keyDown(gi->keyboard.a)) {
			right *= (camera_speed * dt);
			gs->main_camera.pos += right;
		}
		else if (keyDown(gi->keyboard.right) || keyDown(gi->keyboard.d)) {
			right *= (camera_speed * dt);
			gs->main_camera.pos -= right;
		}

		if (keyDown(gi->keyboard.q)) {
			camera->yaw -= rotateSpeed * dt;
		}
		else if (keyDown(gi->keyboard.e)) {
			camera->yaw += rotateSpeed * dt;
		}
		if (keyDown(gi->keyboard.r)) {
			camera->pitch += rotateSpeed * dt;
		}
		else if (keyDown(gi->keyboard.f)) {
			camera->pitch -= rotateSpeed * dt;
		}

		if (camera->pitch > 89.0f) {
			camera->pitch = 89.0f;
		}
		else if (camera->pitch < -89.0f) {
			camera->pitch = -89.0f;
		}

		dir = Vec3(
			cosf(DegToRad(camera->yaw)) * cosf(DegToRad(camera->pitch)),
			sinf(DegToRad(camera->pitch)),
			sinf(DegToRad(camera->yaw)) * cosf(DegToRad(camera->pitch))
		);
		camera->dir = Normalize(dir);

		gs->main_camera.view = LookAtMat(gs->main_camera.pos, gs->main_camera.pos + gs->main_camera.dir, gs->main_camera.up);
		gs->main_camera.inv_view = InverseLookAtMat(gs->main_camera.pos, gs->main_camera.pos + gs->main_camera.dir, gs->main_camera.up);
	}
	// end Camera Update
	// ========================================================================
	// Game Updates
	
	if (keyPressed(gi->mouse.left)) {
		MouseRayReturn mrr = MouseRay(gs, gi);

		// calculate the closest entity in the path of the ray
		// entity_index -1 means no entity was hit, 0 -> gs->num_entities-1 means an entity was hit
		float min = INFINITY;
		int entity_index = -1;
		vec3 quad_norm = -gs->main_camera.dir;
		for (int i = 1; i < gs->entities.size(); i++) {
			vec3 pos = { gs->c_transforms[gs->entities[i].transform].game_pos.x,
				0.0f, gs->c_transforms[gs->entities[i].transform].game_pos.y };
			float new_min = fabsf(Distance(mrr.start, pos));
			if (RayQuadCollisionCheck(mrr.start, mrr.direction, pos, quad_norm)
				&& new_min < min) {
				min = new_min;
				entity_index = i;
			}
		}

		// if we didn't hit any entity and we already selected an entity, check if we hit the ground plane, to position the crosshair
		if (min == INFINITY && 0 <= gs->selected_entity && gs->selected_entity < gs->entities.size()) {
			// we only want to position the crosshair when we have selected a unit since only units can move
			if (gs->entities[gs->selected_entity].unit >= 0) {
				vec3 clicked_point = RayPlaneCollisionCheck(mrr.start, mrr.direction, Vec3(0.0f, -0.4f, 0.0f), UpVec());
				
				gs->c_units[gs->entities[gs->selected_entity].unit].waypoint_pos.clear();
				gs->c_units[gs->entities[gs->selected_entity].unit].waypoint_pos.push_back({ clicked_point.x, clicked_point.z });
				gs->c_units[gs->entities[gs->selected_entity].unit].waypoint_active = true;
			}
			// otherwise we should deselect the entity that isn't a unit
			else {
				gs->selected_entity = -1;
			}
		}
		// if there is already a selected entity moving towards an active waypoint
		// and the user clicks an entity, select the new entity
		else if (entity_index != -1 && 0 <= gs->selected_entity && gs->selected_entity < gs->entities.size()) {
			if (entity_index != gs->selected_entity) {
				gs->selected_entity = entity_index;
			}
		}
		// if we did hit an entity, check if we hit the crosshair, and discard that case
		// otherwise set the selected entity to the hit entity
		else {
			gs->selected_entity = entity_index;
		}
	}
	
	if (keyPressed(gi->mouse.right)) {
		MouseRayReturn mrr = MouseRay(gs, gi);

		// if we didn't hit any entity and we already selected an entity, check if we hit the ground plane, to position the crosshair
		if (0 <= gs->selected_entity && gs->selected_entity < gs->entities.size()) {
			// we only want to position the crosshair when we have selected a unit since only units can move
			if (gs->entities[gs->selected_entity].unit >= 0) {
				vec3 clicked_point = RayPlaneCollisionCheck(mrr.start, mrr.direction, Vec3(0.0f, -0.4f, 0.0f), UpVec());
				gs->c_units[gs->entities[gs->selected_entity].unit].waypoint_pos.push_back({ clicked_point.x, clicked_point.z });
				gs->c_units[gs->entities[gs->selected_entity].unit].waypoint_active = true;
			}
		}
	}

	if (gs->game_ticked) {
		for (int i = 0; i < gs->entities.size(); i++) {
			int unit = gs->entities[i].unit;
			int transform = gs->entities[i].transform;

			if (unit >= 0) {
				if (gs->c_units[unit].waypoint_active) {
					float dist = Distance(gs->c_transforms[transform].game_pos, gs->c_units[unit].waypoint_pos.front());
					if (dist > (dt * 10.0f)) {
						vec2 dir = Normalize(gs->c_units[unit].waypoint_pos.front() - gs->c_transforms[transform].game_pos);
						gs->c_transforms[transform].game_pos += dir * (10.0f * dt);
					}
					else {
						gs->c_units[unit].waypoint_pos.pop_front();
						
						if (gs->c_units[unit].waypoint_pos.empty()) {
							gs->c_units[unit].waypoint_active = false;
						}
					}
				}

				for (int item = 0; item < gs->entities.size(); item++) {
					if (i == item) continue;

					float dist = Distance(gs->c_transforms[transform].game_pos, gs->c_transforms[gs->entities[item].transform].game_pos);
					if (gs->entities[item].is_active && (gs->entities[item].item >= 0) && dist < 1.0f) {
						PickupItem(gs, &gs->entities[i], &gs->entities[item]);
					}
				}
			}
		}
		gs->game_ticked = false;
	}

	// end Game Updates
	// ========================================================================

	// Debug Text to draw to screen
	{
		snprintf(game_debug_text, 1024, "Camera: (%.2f, %.2f, %.2f)\nMouse: (%d, %d)\nDT: %.4f, FPS: %.2f\n",
			camera->pos.x, camera->pos.y, camera->pos.z, gi->mouse.x, gi->mouse.y, dt, 1.0f / dt);
	}

	if (keyPressed(gi->keyboard.m)) {
		__debugbreak();
	}
}