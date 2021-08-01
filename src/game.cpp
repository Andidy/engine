#include "game.h"

void InitGameState(Memory* gameMemory, vec2 windowDimensions) {
	GameState* gs = (GameState*)gameMemory->data;

	gs->resourceAllocator = PermanentResourceAllocator(Megabytes(64));

	gs->numEntities = 0;

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

	gs->numEntities = 12;

	gs->mainCamera.pos = Vec3(0.0f, 0.0f, 1.0f);
	gs->mainCamera.dir = Vec3(0.0f, 0.0f, -1.0f);
	gs->mainCamera.up = Vec3(0.0, 1.0, 0.0f);

	gs->mainCamera.pitch = 0.0f;
	gs->mainCamera.yaw = 0.0f;

	gs->mainCamera.view = LookAtMat(gs->mainCamera.pos, AddVec(gs->mainCamera.pos, gs->mainCamera.dir), gs->mainCamera.up);
	gs->mainCamera.inv_view = InverseLookAtMat(gs->mainCamera.pos, AddVec(gs->mainCamera.pos, gs->mainCamera.dir), gs->mainCamera.up);
	gs->mainCamera.proj = PerspectiveMat(90.0f, windowDimensions.x / windowDimensions.y, 0.1f, 1000.0f);

	Viewport viewport = {};
	viewport.pos = Vec2(0.0f, 0.0f);
	viewport.size = Vec2(windowDimensions.x, windowDimensions.y);
	viewport.depth = Vec2(0.0f, 1.0f);
	gs->mainCamera.viewport = viewport;
}

void GameUpdate(Memory* gameMemory, Input* gameInput, f32 dt, char* gameDebugText) {
	GameState* gameState = (GameState*)gameMemory->data;

	// ========================================================================
	// Camera Update
	Camera* camera = &gameState->mainCamera;
	{
		const f32 baseCameraSpeed = 0.0025f;
		const f32 speedMultiplier = 50.0f;
		f32 cameraSpeed = baseCameraSpeed;

		if (keyDown(gameInput->keyboard.space)) {
			cameraSpeed *= speedMultiplier;
		}

		const f32 rotateSpeed = 0.05f;

		vec3 dir = NormVec(camera->dir);
		vec3 right = NormVec(Cross(camera->up, dir));

		if (keyDown(gameInput->keyboard.w)) {
			vec3 temp_dir = SubVec(dir, ScaleVec(UpVec(), (Dot(dir, UpVec()) / powf(VecLen(dir), 2.0f))));
			temp_dir = ScaleVec(NormVec(temp_dir), cameraSpeed * dt);
			camera->pos = AddVec(camera->pos, temp_dir);
		}
		else if (keyDown(gameInput->keyboard.s)) {
			vec3 temp_dir = SubVec(dir, ScaleVec(UpVec(), (Dot(dir, UpVec()) / powf(VecLen(dir), 2.0f))));
			temp_dir = ScaleVec(NormVec(temp_dir), cameraSpeed * dt);
			camera->pos = AddVec(camera->pos, NegVec(temp_dir));
		}
		else if (keyDown(gameInput->keyboard.up)) {
			dir = ScaleVec(dir, cameraSpeed * dt);
			gameState->mainCamera.pos = AddVec(gameState->mainCamera.pos, dir);
		}
		else if (keyDown(gameInput->keyboard.down)) {
			dir = NegVec(ScaleVec(dir, cameraSpeed * dt));
			gameState->mainCamera.pos = AddVec(gameState->mainCamera.pos, dir);
		}

		if (keyDown(gameInput->keyboard.t)) {
			camera->pos = AddVec(camera->pos, ScaleVec(UpVec(), cameraSpeed * dt));
		}
		else if (keyDown(gameInput->keyboard.g)) {
			camera->pos = AddVec(camera->pos, NegVec(ScaleVec(UpVec(), cameraSpeed * dt)));
		}

		if (keyDown(gameInput->keyboard.left) || keyDown(gameInput->keyboard.a)) {
			right = ScaleVec(right, cameraSpeed * dt);
			gameState->mainCamera.pos = AddVec(gameState->mainCamera.pos, right);
		}
		else if (keyDown(gameInput->keyboard.right) || keyDown(gameInput->keyboard.d)) {
			right = NegVec(ScaleVec(right, cameraSpeed * dt));
			gameState->mainCamera.pos = AddVec(gameState->mainCamera.pos, right);
		}

		if (keyDown(gameInput->keyboard.q)) {
			camera->yaw -= rotateSpeed * dt;
		}
		else if (keyDown(gameInput->keyboard.e)) {
			camera->yaw += rotateSpeed * dt;
		}
		if (keyDown(gameInput->keyboard.r)) {
			camera->pitch += rotateSpeed * dt;
		}
		else if (keyDown(gameInput->keyboard.f)) {
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

		gameState->mainCamera.view = LookAtMat(gameState->mainCamera.pos, AddVec(gameState->mainCamera.pos, gameState->mainCamera.dir), gameState->mainCamera.up);
		gameState->mainCamera.inv_view = InverseLookAtMat(gameState->mainCamera.pos, AddVec(gameState->mainCamera.pos, gameState->mainCamera.dir), gameState->mainCamera.up);
	}
	// end Camera Update
	// ========================================================================
	// Object Picking

	bool hit_object = false;

	if (keyPressed(gameInput->mouse.left)) {
		// calculate the start point and direction vector of the picking ray
		float m00 = gameState->mainCamera.proj.data[0][0];
		float m11 = gameState->mainCamera.proj.data[1][1];
		vec4 camera_space_vector = {
			(2.0f * ((f32)gameInput->mouse.x) / gameState->mainCamera.viewport.size.x - 1.0f) / m00,
			-(2.0f * ((f32)gameInput->mouse.y) / gameState->mainCamera.viewport.size.y - 1.0f) / m11,
			-1.0f,
			0.0f
		};

		vec4 dir = MulMatVec(gameState->mainCamera.inv_view, camera_space_vector);
		vec3 direction_vector = NormVec({ dir.x, dir.y, dir.z });
		vec3 start_vector = { 
			gameState->mainCamera.inv_view.data[0][3],
			gameState->mainCamera.inv_view.data[1][3],
			gameState->mainCamera.inv_view.data[2][3]
		};
		
		// now compare the picking ray against the hitboxes of the models in the scene
		vec3 pos = gameState->quad.renderPos;
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
		gameState->quad.renderPos.y += 1.0f;
	}

	// end Object Picking
	// ========================================================================

	// Debug Text to draw to screen
	{
		snprintf(gameDebugText, 1024, "Camera: (%.2f, %.2f, %.2f)\nMouse: (%d, %d)\n", camera->pos.x, camera->pos.y, camera->pos.z, gameInput->mouse.x, gameInput->mouse.y);
	}
}