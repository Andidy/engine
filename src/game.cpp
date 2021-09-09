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

/*
// The return type is the index of the entity the ray connected with in the gs->entities array, -1 means no entity was hit.
int RayEntityCollisionCheck(GameState* gs, vec3 start, vec3 direction) {
	int result = -1;

	// now compare the picking ray against the hitboxes of the models in the scene
	float min_distance = INFINITY;
	for (int i = 0; i < gs->num_entities; i++) {
		vec3 pos = gs->entities[i].render_pos;
		float radius = 1.0f;

		vec3 pos_to_start = SubVec(start, pos);

		float b = Dot(ScaleVec(direction, 2.0f), pos_to_start);
		float c = Dot(pos_to_start, pos_to_start) - radius * radius;

		float discriminant = b * b - 4.0f * c;
		float t0 = -1.0f;
		float t1 = -1.0f;
		if (discriminant > 0) {
			// 2 real roots, ray goes through the sphere
			t0 = (-b + sqrtf(discriminant)) / 2.0f;
			t1 = (-b - sqrtf(discriminant)) / 2.0f;
		}
		else if (-0.0001f < discriminant && discriminant < 0.0001f) {
			// double root, ray is tangent to the sphere
			t0 = -b / 2.0f;
		}
		else { // discriminant < 0
			// no roots, ray does not intersect the sphere
			DebugPrint((char*)"Didn't Hit Object\n");
			continue;
		}

		float t = fmaxf(t0, t1);
		float distance = Distance(start, pos);
		if (t > 0.0f && t > 0.1f && t < 1000.0f && distance < min_distance) {
			// we hit the object and it is within the bounds of the 
			// near and far of the projection matrix and it is the
			// closest object that we have hit
			DebugPrint((char*)"Better Object Hit\n");
			result = i;
			min_distance = distance;
		}
		else {
			DebugPrint((char*)"Worse Object Hit\n");
		}
	}

	gs->picking_dist = min_distance;
	return result;
}
*/

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

void SetMainCameraViewportAndProjMat(Memory* game_memory, float width, float height) {
	GameState* gs = (GameState*)game_memory->data;

	gs->main_camera.viewport.size.x = width;
	gs->main_camera.viewport.size.y = height;
	gs->main_camera.proj = PerspectiveMat(90.0f, width / height, 0.1f, 1000.0f);
}

void InitGameState(Memory* game_memory, vec2 window_dimensions, AssetHandle* asset_handles) {
	GameState* gs = (GameState*)game_memory->data;

	gs->resource_allocator = PermanentResourceAllocator(Megabytes(64));

	gs->MAX_ENTITIES = 1024;
	//gs->entities = (Entity*)gs->resource_allocator.Allocate((i64)(gs->MAX_ENTITIES * sizeof(Entity)));
	//gs->entities = (Entity*)calloc(gs->MAX_ENTITIES, sizeof(Entity));
	gs->entities = new Entity[gs->MAX_ENTITIES];

	gs->num_entities = 0;

	// Experimental asset loading

	LoadGameAssets(gs, asset_handles);

	// end Experimental asset loading

	for (int i = 0; i < gs->num_entities; i++) {
		if (gs->entities[i].name.compare((char*)"crosshair_0") == 0) {
			gs->crosshair_entity = i;
			gs->entities[i].should_render = false;
			break;
		}
	}

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

void GameUpdate(Memory* game_memory, Input* gi, f32 dt, char* game_debug_text) {
	GameState* gs = (GameState*)game_memory->data;

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
	// Object Picking
	/*
	if (keyPressed(gi->mouse.middle) && gs->picked_object >= 0) {
		gs->picked_object = -1;
		gs->picking_dir = ZeroVec();
	}
	else if (keyPressed(gi->mouse.middle) && gs->picked_object == -1) {
		MouseRayReturn mrr = MouseRay(gs, gi);

		gs->picked_object = RayEntityCollisionCheck(gs, mrr.start, mrr.direction);
		if (gs->picked_object > 0) {
			// store the direction vector for cross frame object selection
			gs->picking_dir = mrr.direction;
		}
		else {
			gs->picking_dir = ZeroVec();
		}
	}

	if (gs->picked_object != -1) {
		MouseRayReturn mrr = MouseRay(gs, gi);
		gs->entities[gs->picked_object].render_pos = AddVec(mrr.start, ScaleVec(mrr.direction, gs->picking_dist));
	}
	*/
	// end Object Picking
	// ========================================================================
	// Game Updates
	
	if (keyPressed(gi->mouse.left)) {
		MouseRayReturn mrr = MouseRay(gs, gi);

		// calculate the closest entity in the path of the ray
		// entity_index -1 means no entity was hit, 0 -> gs->num_entities-1 means an entity was hit
		float min = INFINITY;
		int entity_index = -1;
		vec3 quad_norm = -gs->main_camera.dir;
		for (int i = 1; i < gs->num_entities; i++) {
			vec3 pos = { gs->entities[i].game_pos.x, 0.0f, gs->entities[i].game_pos.y };
			float new_min = fabsf(Distance(mrr.start, pos));
			if (RayQuadCollisionCheck(mrr.start, mrr.direction, pos, quad_norm)
				&& new_min < min) {
				min = new_min;
				entity_index = i;
			}
		}

		// if we didn't hit any entity and we already selected an entity, check if we hit the ground plane, to position the crosshair
		if (min == INFINITY && 0 <= gs->selected_entity && gs->selected_entity < gs->num_entities) {
			// we only want to position the crosshair when we have selected a unit since only units can move
			if (gs->entities[gs->selected_entity].is_unit) {
				vec3 clicked_point = RayPlaneCollisionCheck(mrr.start, mrr.direction, Vec3(0.0f, -0.4f, 0.0f), UpVec());
				gs->entities[gs->selected_entity].waypoint_pos = { clicked_point.x, clicked_point.z };
				gs->entities[gs->selected_entity].waypoint_active = true;
			}
			// otherwise we should deselect the entity that isn't a unit
			else {
				gs->selected_entity = -1;
			}
		}
		// if there is already a selected entity moving towards an active waypoint
		// and the user clicks an entity, select the new entity
		else if (entity_index != -1 && 0 <= gs->selected_entity && gs->selected_entity < gs->num_entities) {
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

	if (gs->game_ticked) {
		for (int i = 0; i < gs->num_entities; i++) {
			if (gs->entities[i].is_unit && gs->entities[i].waypoint_active) {
				float dist = Distance(gs->entities[i].game_pos, gs->entities[i].waypoint_pos);
				if (dist > (dt * 10.0f)) {
					vec2 dir = Normalize(gs->entities[i].waypoint_pos - gs->entities[i].game_pos);
					gs->entities[i].game_pos += dir * (10.0f * dt);
				}
				else {
					gs->entities[i].waypoint_active = false;
				}
			}
		}
		gs->game_ticked = false;
	}

	// end Game Updates
	// ========================================================================

	// Debug Text to draw to screen
	{
		const char* selected_entity = NULL;
		if (0 <= gs->selected_entity && gs->selected_entity < gs->num_entities) {
			selected_entity = gs->entities[gs->selected_entity].name.c_str();
		}
		else {
			selected_entity = "No entity selected\0";
		}

		snprintf(game_debug_text, 1024, "Camera: (%.2f, %.2f, %.2f)\nMouse: (%d, %d)\nDT: %.4f, FPS: %.2f\nSelected Entity: %s\n",
			camera->pos.x, camera->pos.y, camera->pos.z, gi->mouse.x, gi->mouse.y, dt, 1.0f / dt, selected_entity);
	}

	if (keyPressed(gi->keyboard.m)) {
		__debugbreak();
	}
}