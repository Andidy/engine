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

	gameState->mainCamera.pos = Vec3(0.0f, 10.0f, 0.0f);
	gameState->mainCamera.target = Vec3(0.0f, 0.0f, 0.0f);
	gameState->mainCamera.up = Vec3(0.0, 0.0, 1.0f);
	gameState->mainCamera.view = LookAtMat(gameState->mainCamera.pos, gameState->mainCamera.target, gameState->mainCamera.up);
	gameState->mainCamera.proj = PerspectiveMat(90.0f, windowDimensions.x / windowDimensions.y, 0.0001f, 1000.0f);
}

void GameUpdate(Memory* gameMemory, Input* gameInput) {
	GameState* gameState = (GameState*)gameMemory->data;

	if (keyReleased(gameInput->keyboard.w)) {
		gameState->blackGuyHead.renderPos.z += 1;
	}
	else if (keyReleased(gameInput->keyboard.s)) {
		gameState->blackGuyHead.renderPos.z -= 1;
	}
	else if (keyReleased(gameInput->keyboard.a)) {
		gameState->blackGuyHead.renderPos.x -= 1;
	}
	else if (keyReleased(gameInput->keyboard.d)) {
		gameState->blackGuyHead.renderPos.x += 1;
	}
}