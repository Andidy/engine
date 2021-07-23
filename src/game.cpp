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
	gs->mainCamera.proj = PerspectiveMat(90.0f, windowDimensions.x / windowDimensions.y, 0.1f, 1000.0f);
}

void GameUpdate(Memory* gameMemory, Input* gameInput, f32 dt, char* gameDebugText) {
	GameState* gameState = (GameState*)gameMemory->data;

	// ========================================================================
	// Camera Update

	Camera* camera = &gameState->mainCamera;

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
	
	// end Camera Update
	// ========================================================================
	// Object Picking

	// viewport width and height are needed for this calculation
	// m11 = cotan(fovy / 2) / aspect_ratio
	// m22 = cotan(fovy / 2)
	// vec3 mouse_vector = Vec3((f32)gameInput->mouse.x, (f32)gameInput->mouse.y, 1.0f);
	// vec4 camera_space_vector = Vec4((2.0f * mouse_vector.x / viewport_width - 1.0f) / m11,
	//								  (-2.0f * mouse_vector.y / viewport_height + 1.0f) / m22,
	//								  -1.0f,
	//									0.0f
	//								);

	// mat4 inv_view = InverseViewMat(gameState->mainCamera.view);
	// vec4 direction_vector = MulMatVec(inv_view, camera_space_vector);

	// end Object Picking
	// ========================================================================

	// Debug Text to draw to screen
	{
		snprintf(gameDebugText, 1024, "Camera: (%.2f, %.2f, %.2f)\nMouse: (%d, %d)\n", camera->pos.x, camera->pos.y, camera->pos.z, gameInput->mouse.x, gameInput->mouse.y);
	}
}