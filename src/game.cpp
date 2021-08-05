#include "game.h"

void InitGameState(Memory* game_memory, vec2 window_dimensions) {
	GameState* gs = (GameState*)game_memory->data;

	gs->resource_allocator = PermanentResourceAllocator(Megabytes(64));

	gs->num_entities = 0;

	gs->blackGuyHead = { Vec3(-1.0f, 0.0f, -1.0f), Vec3(1.0f, 1.0f, 1.0f), UpVec(), 0.0f };
	gs->blackGuyHead2 = { Vec3(-2.0f, 0.0f, -1.0f), Vec3(1.0f, 1.0f, 1.0f), UpVec(), 90.0f };
	gs->bunnyTest = { Vec3(-3.0f, 0.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gs->bunnyTest2 = { Vec3(-4.0f, 0.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 180.0f };

	gs->cubes[0] = { Vec3(-1.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gs->cubes[1] = { Vec3(-2.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gs->cubes[2] = { Vec3(-3.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gs->cubes[3] = { Vec3(-4.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gs->cubes[4] = { Vec3(-5.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gs->cubes[5] = { Vec3(-6.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gs->cubes[6] = { Vec3(-7.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };

	gs->quad = { Vec3(-5.0f, 3.0f, -3.0f), OneVec(), UpVec(), 0.0f };

	gs->num_entities = 12;

	gs->main_camera.pos = Vec3(0.0f, 0.0f, 1.0f);
	gs->main_camera.dir = Vec3(0.0f, 0.0f, -1.0f);
	gs->main_camera.up = Vec3(0.0, 1.0, 0.0f);

	gs->main_camera.pitch = 0.0f;
	gs->main_camera.yaw = 0.0f;

	gs->main_camera.view = LookAtMat(gs->main_camera.pos, AddVec(gs->main_camera.pos, gs->main_camera.dir), gs->main_camera.up);
	gs->main_camera.inv_view = InverseLookAtMat(gs->main_camera.pos, AddVec(gs->main_camera.pos, gs->main_camera.dir), gs->main_camera.up);
	gs->main_camera.proj = PerspectiveMat(90.0f, window_dimensions.x / window_dimensions.y, 0.1f, 1000.0f);

	Viewport viewport = {};
	viewport.pos = Vec2(0.0f, 0.0f);
	viewport.size = Vec2(window_dimensions.x, window_dimensions.y);
	viewport.depth = Vec2(0.0f, 1.0f);
	gs->main_camera.viewport = viewport;
}

void GameUpdate(Memory* game_memory, Input* game_input, f32 dt, char* game_debug_text) {
	GameState* gameState = (GameState*)game_memory->data;

	// ========================================================================
	// Camera Update
	Camera* camera = &gameState->main_camera;
	{
		const f32 base_camera_speed = 0.0025f;
		const f32 speed_multiplier = 50.0f;
		f32 camera_speed = base_camera_speed;

		if (keyDown(game_input->keyboard.space)) {
			camera_speed *= speed_multiplier;
		}

		const f32 rotateSpeed = 0.05f;

		vec3 dir = NormVec(camera->dir);
		vec3 right = NormVec(Cross(camera->up, dir));

		if (keyDown(game_input->keyboard.w)) {
			vec3 temp_dir = SubVec(dir, ScaleVec(UpVec(), (Dot(dir, UpVec()) / powf(VecLen(dir), 2.0f))));
			temp_dir = ScaleVec(NormVec(temp_dir), camera_speed * dt);
			camera->pos = AddVec(camera->pos, temp_dir);
		}
		else if (keyDown(game_input->keyboard.s)) {
			vec3 temp_dir = SubVec(dir, ScaleVec(UpVec(), (Dot(dir, UpVec()) / powf(VecLen(dir), 2.0f))));
			temp_dir = ScaleVec(NormVec(temp_dir), camera_speed * dt);
			camera->pos = AddVec(camera->pos, NegVec(temp_dir));
		}
		else if (keyDown(game_input->keyboard.up)) {
			dir = ScaleVec(dir, camera_speed * dt);
			gameState->main_camera.pos = AddVec(gameState->main_camera.pos, dir);
		}
		else if (keyDown(game_input->keyboard.down)) {
			dir = NegVec(ScaleVec(dir, camera_speed * dt));
			gameState->main_camera.pos = AddVec(gameState->main_camera.pos, dir);
		}

		if (keyDown(game_input->keyboard.t)) {
			camera->pos = AddVec(camera->pos, ScaleVec(UpVec(), camera_speed * dt));
		}
		else if (keyDown(game_input->keyboard.g)) {
			camera->pos = AddVec(camera->pos, NegVec(ScaleVec(UpVec(), camera_speed * dt)));
		}

		if (keyDown(game_input->keyboard.left) || keyDown(game_input->keyboard.a)) {
			right = ScaleVec(right, camera_speed * dt);
			gameState->main_camera.pos = AddVec(gameState->main_camera.pos, right);
		}
		else if (keyDown(game_input->keyboard.right) || keyDown(game_input->keyboard.d)) {
			right = NegVec(ScaleVec(right, camera_speed * dt));
			gameState->main_camera.pos = AddVec(gameState->main_camera.pos, right);
		}

		if (keyDown(game_input->keyboard.q)) {
			camera->yaw -= rotateSpeed * dt;
		}
		else if (keyDown(game_input->keyboard.e)) {
			camera->yaw += rotateSpeed * dt;
		}
		if (keyDown(game_input->keyboard.r)) {
			camera->pitch += rotateSpeed * dt;
		}
		else if (keyDown(game_input->keyboard.f)) {
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
		camera->dir = NormVec(dir);

		gameState->main_camera.view = LookAtMat(gameState->main_camera.pos, AddVec(gameState->main_camera.pos, gameState->main_camera.dir), gameState->main_camera.up);
		gameState->main_camera.inv_view = InverseLookAtMat(gameState->main_camera.pos, AddVec(gameState->main_camera.pos, gameState->main_camera.dir), gameState->main_camera.up);
	}
	// end Camera Update
	// ========================================================================
	// Object Picking

	bool hit_object = false;

	if (keyPressed(game_input->mouse.left)) {
		// calculate the start point and direction vector of the picking ray
		float m00 = gameState->main_camera.proj.data[0][0];
		float m11 = gameState->main_camera.proj.data[1][1];
		vec4 camera_space_vector = {
			(2.0f * ((f32)game_input->mouse.x) / gameState->main_camera.viewport.size.x - 1.0f) / m00,
			-(2.0f * ((f32)game_input->mouse.y) / gameState->main_camera.viewport.size.y - 1.0f) / m11,
			-1.0f,
			0.0f
		};

		vec4 dir = MulMatVec(gameState->main_camera.inv_view, camera_space_vector);
		vec3 direction_vector = NormVec({ dir.x, dir.y, dir.z });
		vec3 start_vector = { 
			gameState->main_camera.inv_view.data[0][3],
			gameState->main_camera.inv_view.data[1][3],
			gameState->main_camera.inv_view.data[2][3]
		};
		
		// now compare the picking ray against the hitboxes of the models in the scene
		vec3 pos = gameState->quad.render_pos;
		float radius = 1.0f;
		
		vec3 pos_to_start = SubVec(start_vector, pos);

		// float a = 1;
		float b = Dot(ScaleVec(direction_vector, 2.0f), pos_to_start);
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
		else /* discriminant < 0 */ {
			// no roots, ray does not intersect the sphere
		}

		float t = fmaxf(t0, t1);
		if (t > 0.0f && t > 0.1f && t < 1000.0f) {
			// we hit the object and it is within the bounds of the 
			// near and far of the projection matrix
			hit_object = true;
			DebugPrint((char*)"Hit Object");
		}
		else {
			DebugPrint((char*)"Didn't Hit Object");
		}
	}

	if (hit_object) {
		gameState->quad.render_pos.y += 1.0f;
	}

	// end Object Picking
	// ========================================================================

	// Debug Text to draw to screen
	{
		snprintf(game_debug_text, 1024, "Camera: (%.2f, %.2f, %.2f)\nMouse: (%d, %d)\n", camera->pos.x, camera->pos.y, camera->pos.z, game_input->mouse.x, game_input->mouse.y);
	}
}