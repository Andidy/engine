#include "game.h"

void GenerateTerrain(GameMap* gameMap, PermanentResourceAllocator* allocator) {
	// const int numTerraces = 15;
	gameMap->tiles = (Tile*)allocator->Allocate(sizeof(Tile) * gameMap->mapWidth * gameMap->mapHeight);

	for (int y = 0; y < gameMap->mapHeight; y++) {
		for (int x = 0; x < gameMap->mapWidth; x++) {
			f32 nx = (f32)x / (f32)gameMap->mapWidth - 0.5f;
			f32 ny = (f32)y / (f32)gameMap->mapHeight - 0.5f;
			f32 e = 0;
			f32 f = 0;
			for (int n = 1; n <= 5; n++) {
				f += (1.0f / n);
				e += f * noise(n * nx, n * ny);
			}
			e /= f;
			// gameMap->tiles[x + y * gameMap->mapWidth].elevation = ((int)(e * (f32)numTerraces)) / numTerraces;
			gameMap->tiles[x + y * gameMap->mapWidth].elevation = (int)(e * 100.0f);
		}
	}
}

void InitGameState(Memory* gameMemory, vec2 windowDimensions) {
	GameState* gameState = (GameState*)gameMemory->data;

	gameState->resourceAllocator = PermanentResourceAllocator(Megabytes(64));

	gameState->blackGuyHead = { Vec3(-1.0f, 0.0f, -1.0f), Vec3(1.0f, 1.0f, 1.0f) };
	
	gameState->gameMap.ent = { Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.001f, 1.0f) };
	gameState->gameMap.mapWidth = 20;
	gameState->gameMap.mapHeight = 10;
	GenerateTerrain(&gameState->gameMap, &gameState->resourceAllocator);

	gameState->mainCamera.pos = Vec3(0.0f, 0.0f, 1.0f);
	gameState->mainCamera.dir = Vec3(0.0f, 0.0f, -1.0f);
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
		vec3 temp_dir = SubVec(dir, ScaleVec(UpVec(), (Dot(dir, UpVec()) / powf(VecLen(dir) , 2.0f))));
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

	if (keyDown(gameInput->keyboard.right) || keyDown(gameInput->keyboard.d)) {
		right = ScaleVec(right, cameraSpeed * dt);
		gameState->mainCamera.pos = AddVec(gameState->mainCamera.pos, right);
	}
	else if (keyDown(gameInput->keyboard.left) || keyDown(gameInput->keyboard.a)) {
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

	char debug_str[256];
	snprintf(debug_str, 256, "Camera: (%f, %f, %f)\n", camera->pos.x, camera->pos.y, camera->pos.z);
	//DebugPrint(debug_str);

	// end Camera Update
	// ========================================================================
}