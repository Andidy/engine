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
	gameState->mainCamera.target = Vec3(0.0f, 0.0f, 0.0f);
	gameState->mainCamera.up = Vec3(0.0, 1.0, 0.0f);

	gameState->mainCamera.orientation = { 0, 0, 0, 1 };
	gameState->mainCamera.orientation = RotateQuat(gameState->mainCamera.orientation, UpVec(), 180);

	// gameState->mainCamera.view = LookAtMat(gameState->mainCamera.pos, gameState->mainCamera.target, gameState->mainCamera.up);
	gameState->mainCamera.view = ViewMatFromQuat(gameState->mainCamera.orientation, gameState->mainCamera.pos);
	gameState->mainCamera.proj = PerspectiveMat(90.0f, windowDimensions.x / windowDimensions.y, 0.0001f, 1000.0f);
}

void GameUpdate(Memory* gameMemory, Input* gameInput, f32 dt) {
	GameState* gameState = (GameState*)gameMemory->data;

	if (keyDown(gameInput->keyboard.w)) {
		vec3 dir = ZeroVec();
		dir.x = gameState->mainCamera.orientation.x;
		dir.y = gameState->mainCamera.orientation.y;
		dir.z = gameState->mainCamera.orientation.z;
		dir = NormVec(dir);
		dir = ScaleVec(dir, dt / 1000.0f);
		gameState->mainCamera.pos.x += dir.x;
		gameState->mainCamera.pos.y += dir.y;
		gameState->mainCamera.pos.z += dir.z;
	}
	else if (keyDown(gameInput->keyboard.s)) {
		vec3 dir = ZeroVec();
		dir.x = gameState->mainCamera.orientation.x;
		dir.y = gameState->mainCamera.orientation.y;
		dir.z = gameState->mainCamera.orientation.z;
		dir = NormVec(dir);
		dir = ScaleVec(dir, dt / 1000.0f);
		dir = NegVec(dir);
		gameState->mainCamera.pos.x += dir.x;
		gameState->mainCamera.pos.y += dir.y;
		gameState->mainCamera.pos.z += dir.z;
	}
	else if (keyReleased(gameInput->keyboard.a)) {
		gameState->blackGuyHead.renderPos.x -= 1;
	}
	else if (keyReleased(gameInput->keyboard.d)) {
		gameState->blackGuyHead.renderPos.x += 1;
	}

	const f32 rotateSpeed = 10;
	if (keyDown(gameInput->keyboard.q)) {
		gameState->mainCamera.orientation = RotateQuat(gameState->mainCamera.orientation, UpVec(), DegToRad(-1 * rotateSpeed * dt));
	}
	else if (keyDown(gameInput->keyboard.e)) {
		gameState->mainCamera.orientation = RotateQuat(gameState->mainCamera.orientation, UpVec(), DegToRad(rotateSpeed * dt));
	}
	else if (keyDown(gameInput->keyboard.r)) {
		mat4 temp = RotMatFromQuat(gameState->mainCamera.orientation);
		vec3 axis = ZeroVec();
		axis.x = temp.data[0][0];
		axis.y = temp.data[1][0];
		axis.z = temp.data[2][0];
		gameState->mainCamera.orientation = RotateQuat(gameState->mainCamera.orientation, axis, DegToRad(-1 * rotateSpeed * dt));
	}
	else if (keyDown(gameInput->keyboard.f)) {
		mat4 temp = RotMatFromQuat(gameState->mainCamera.orientation);
		vec3 axis = ZeroVec();
		axis.x = temp.data[0][0];
		axis.y = temp.data[1][0];
		axis.z = temp.data[2][0];
		gameState->mainCamera.orientation = RotateQuat(gameState->mainCamera.orientation, axis, DegToRad(rotateSpeed * dt));
	}

	gameState->mainCamera.view = ViewMatFromQuat(gameState->mainCamera.orientation, gameState->mainCamera.pos);
}