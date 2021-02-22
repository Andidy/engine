#include "game.h"

void GenerateTerrain(GameMap* gameMap, PermanentResourceAllocator* allocator) {
	const int width = 100;
	const int height = 75;
	const int numTerraces = 15;
	gameMap->elevationMap = (int*)allocator->Allocate(sizeof(int) * width * height);
	gameMap->mapWidth = width;
	gameMap->mapHeight = height;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			f32 nx = (f32)x / (f32)width - 0.5f;
			f32 ny = (f32)y / (f32)height - 0.5f;
			f32 e = 0;
			f32 f = 0;
			for (int n = 1; n <= 5; n++) {
				f += (1.0f / n);
				e += f * noise(n * nx, n * ny);
			}
			e /= f;
			gameMap->elevationMap[x + y * width] = ((int)(e * (f32)numTerraces)) / numTerraces;
		}
	}
}

void InitGameState(Memory* gameMemory, vec2 windowDimensions) {
	GameState* gameState = (GameState*)gameMemory->data;

	gameState->resourceAllocator = PermanentResourceAllocator(Megabytes(1));

	gameState->blackGuyHead = { Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f) };
	
	gameState->gameMap.ent = { Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f) };
	GenerateTerrain(&gameState->gameMap, &gameState->resourceAllocator);

	gameState->mainCamera.pos = Vec3(0.0f, 0.0f, 1.0f);
	gameState->mainCamera.dir = Vec3(0.0f, 0.0f, 0.0f);
	gameState->mainCamera.up = Vec3(0.0, 1.0, 0.0f);

	gameState->mainCamera.pitch = 0.0f;
	gameState->mainCamera.yaw = 0.0f;

	// gameState->mainCamera.view = LookAtMat(gameState->mainCamera.pos, gameState->mainCamera.target, gameState->mainCamera.up);
	gameState->mainCamera.view = LookAtMat(gameState->mainCamera.pos, gameState->mainCamera.dir, gameState->mainCamera.up);
	gameState->mainCamera.proj = PerspectiveMat(90.0f, windowDimensions.x / windowDimensions.y, 0.0001f, 1000.0f);
}

void GameUpdate(Memory* gameMemory, Input* gameInput, f32 dt) {
	GameState* gameState = (GameState*)gameMemory->data;

	// ========================================================================
	// Camera Update

	Camera* camera = &gameState->mainCamera;

	const f32 cameraSpeed = 0.01f;
	const f32 rotateSpeed = 0.25f;
	
	vec3 dir = NormVec(camera->dir);
	vec3 right = NormVec(Cross(camera->up, dir));

	if (keyDown(gameInput->keyboard.w)) {
		dir = ScaleVec(dir, cameraSpeed * dt);
		gameState->mainCamera.pos = AddVec(gameState->mainCamera.pos, dir);
	}
	else if (keyDown(gameInput->keyboard.s)) {
		dir = NegVec(ScaleVec(dir, cameraSpeed * dt));
		gameState->mainCamera.pos = AddVec(gameState->mainCamera.pos, dir);
	}
	if (keyDown(gameInput->keyboard.d)) {
		right = ScaleVec(right, cameraSpeed * dt);
		gameState->mainCamera.pos = AddVec(gameState->mainCamera.pos, right);
	}
	else if (keyDown(gameInput->keyboard.a)) {
		right = NegVec(ScaleVec(right, cameraSpeed * dt));
		gameState->mainCamera.pos = AddVec(gameState->mainCamera.pos, right);
	}

	if (keyDown(gameInput->keyboard.q)) {
		camera->yaw += rotateSpeed * dt;
	}
	else if (keyDown(gameInput->keyboard.e)) {
		camera->yaw -= rotateSpeed * dt;
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
		cos(DegToRad(camera->yaw)) * cos(DegToRad(camera->pitch)),
		sin(DegToRad(camera->pitch)),
		sin(DegToRad(camera->yaw)) * cos(DegToRad(camera->pitch))
	);
	camera->dir = NormVec(dir);

	gameState->mainCamera.view = LookAtMat(gameState->mainCamera.pos, AddVec(gameState->mainCamera.pos, gameState->mainCamera.dir), gameState->mainCamera.up);

	// end Camera Update
	// ========================================================================
}