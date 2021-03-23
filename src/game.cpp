#include "game.h"

ivec2 GetNeighborPosition(GameMap* gameMap, int32_t x, int32_t y, TileNeighbor direction) {
	// a -1 means neighbor is an invalid map position (out of bounds)

	ivec2 neighborPosition = IVec2(-1, -1);

	switch (direction) {
		case TileNeighbor::EAST:
		{
			neighborPosition.y = y;
			if ((x + 1) < gameMap->mapWidth) {
				neighborPosition.x = x + 1;
			}
			else {
				if (gameMap->wrapHorizontal) {
					neighborPosition.x = 0;
				}
				else {
					neighborPosition.x = -1;
				}
			}
		} break;
		case TileNeighbor::NORTHEAST:
		{
			if ((x + 1) < gameMap->mapWidth) {
				neighborPosition.x = x + 1;
			}
			else {
				if (gameMap->wrapHorizontal) {
					neighborPosition.x = 0;
				}
				else {
					neighborPosition.x = -1;
				}
			}

			if ((y + 1) < gameMap->mapHeight) {
				neighborPosition.y = y + 1;
			}
			else {
				if (gameMap->wrapVertical) {
					neighborPosition.y = 0;
				}
				else {
					neighborPosition.y = -1;
				}
			}
		} break;
		case TileNeighbor::NORTH:
		{
			neighborPosition.x = x;
			if ((y + 1) < gameMap->mapHeight) {
				neighborPosition.y = y + 1;
			}
			else {
				if (gameMap->wrapVertical) {
					neighborPosition.y = 0;
				}
				else {
					neighborPosition.y = -1;
				}
			}
		} break;
		case TileNeighbor::NORTHWEST:
		{
			if ((x - 1) >= 0) {
				neighborPosition.x = x - 1;
			}
			else {
				if (gameMap->wrapHorizontal) {
					neighborPosition.x = gameMap->mapWidth - 1;
				}
				else {
					neighborPosition.x = -1;
				}
			}
			
			if ((y + 1) < gameMap->mapHeight) {
				neighborPosition.y = y + 1;
			}
			else {
				if (gameMap->wrapVertical) {
					neighborPosition.y = 0;
				}
				else {
					neighborPosition.y = -1;
				}
			}
		} break;
		case TileNeighbor::WEST:
		{
			neighborPosition.y = y;
			if ((x - 1) >= 0) {
				neighborPosition.x = x - 1;
			}
			else {
				if (gameMap->wrapHorizontal) {
					neighborPosition.x = gameMap->mapWidth - 1;
				}
				else {
					neighborPosition.x = -1;
				}
			}
		} break;
		case TileNeighbor::SOUTHEAST:
		{
			if ((x + 1) < gameMap->mapWidth) {
				neighborPosition.x = x + 1;
			}
			else {
				if (gameMap->wrapHorizontal) {
					neighborPosition.x = 0;
				}
				else {
					neighborPosition.x = -1;
				}
			}

			if ((y - 1) >= 0) {
				neighborPosition.y = y - 1;
			}
			else {
				if (gameMap->wrapVertical) {
					neighborPosition.y = gameMap->mapHeight - 1;
				}
				else {
					neighborPosition.y = -1;
				}
			}
		} break;
		case TileNeighbor::SOUTH:
		{
			neighborPosition.x = x;
			if ((y - 1) >= 0) {
				neighborPosition.y = y - 1;
			}
			else {
				if (gameMap->wrapVertical) {
					neighborPosition.y = gameMap->mapHeight - 1;
				}
				else {
					neighborPosition.y = -1;
				}
			}
		} break;
		case TileNeighbor::SOUTHWEST:
		{
			if ((x - 1) >= 0) {
				neighborPosition.x = x - 1;
			}
			else {
				if (gameMap->wrapHorizontal) {
					neighborPosition.x = gameMap->mapWidth - 1;
				}
				else {
					neighborPosition.x = -1;
				}
			}

			if ((y - 1) >= 0) {
				neighborPosition.y = y - 1;
			}
			else {
				if (gameMap->wrapVertical) {
					neighborPosition.y = gameMap->mapHeight - 1;
				}
				else {
					neighborPosition.y = -1;
				}
			}
		} break;
		default: /* some error? */ break;
	}

	return neighborPosition;
}

ivec2 GetNeighborPosition(GameMap* gameMap, int32_t tileIndex, TileNeighbor direction) {
	// a -1 means neighbor is an invalid map position (out of bounds)
	int32_t x = tileIndex % gameMap->mapWidth;
	int32_t y = tileIndex / gameMap->mapWidth;
	ivec2 neighborPosition = IVec2(-1, -1);

	switch (direction) {
		case TileNeighbor::EAST:
		{
			neighborPosition.y = y;
			if ((x + 1) < gameMap->mapWidth) {
				neighborPosition.x = x + 1;
			}
			else {
				if (gameMap->wrapHorizontal) {
					neighborPosition.x = 0;
				}
				else {
					neighborPosition.x = -1;
				}
			}
		} break;
		case TileNeighbor::NORTHEAST:
		{
			if ((x + 1) < gameMap->mapWidth) {
				neighborPosition.x = x + 1;
			}
			else {
				if (gameMap->wrapHorizontal) {
					neighborPosition.x = 0;
				}
				else {
					neighborPosition.x = -1;
				}
			}

			if ((y + 1) < gameMap->mapHeight) {
				neighborPosition.y = y + 1;
			}
			else {
				if (gameMap->wrapVertical) {
					neighborPosition.y = 0;
				}
				else {
					neighborPosition.y = -1;
				}
			}
		} break;
		case TileNeighbor::NORTH:
		{
			neighborPosition.x = x;
			if ((y + 1) < gameMap->mapHeight) {
				neighborPosition.y = y + 1;
			}
			else {
				if (gameMap->wrapVertical) {
					neighborPosition.y = 0;
				}
				else {
					neighborPosition.y = -1;
				}
			}
		} break;
		case TileNeighbor::NORTHWEST:
		{
			if ((x - 1) >= 0) {
				neighborPosition.x = x - 1;
			}
			else {
				if (gameMap->wrapHorizontal) {
					neighborPosition.x = gameMap->mapWidth - 1;
				}
				else {
					neighborPosition.x = -1;
				}
			}

			if ((y + 1) < gameMap->mapHeight) {
				neighborPosition.y = y + 1;
			}
			else {
				if (gameMap->wrapVertical) {
					neighborPosition.y = 0;
				}
				else {
					neighborPosition.y = -1;
				}
			}
		} break;
		case TileNeighbor::WEST:
		{
			neighborPosition.y = y;
			if ((x - 1) >= 0) {
				neighborPosition.x = x - 1;
			}
			else {
				if (gameMap->wrapHorizontal) {
					neighborPosition.x = gameMap->mapWidth - 1;
				}
				else {
					neighborPosition.x = -1;
				}
			}
		} break;
		case TileNeighbor::SOUTHEAST:
		{
			if ((x + 1) < gameMap->mapWidth) {
				neighborPosition.x = x + 1;
			}
			else {
				if (gameMap->wrapHorizontal) {
					neighborPosition.x = 0;
				}
				else {
					neighborPosition.x = -1;
				}
			}

			if ((y - 1) >= 0) {
				neighborPosition.y = y - 1;
			}
			else {
				if (gameMap->wrapVertical) {
					neighborPosition.y = gameMap->mapHeight - 1;
				}
				else {
					neighborPosition.y = -1;
				}
			}
		} break;
		case TileNeighbor::SOUTH:
		{
			neighborPosition.x = x;
			if ((y - 1) >= 0) {
				neighborPosition.y = y - 1;
			}
			else {
				if (gameMap->wrapVertical) {
					neighborPosition.y = gameMap->mapHeight - 1;
				}
				else {
					neighborPosition.y = -1;
				}
			}
		} break;
		case TileNeighbor::SOUTHWEST:
		{
			if ((x - 1) >= 0) {
				neighborPosition.x = x - 1;
			}
			else {
				if (gameMap->wrapHorizontal) {
					neighborPosition.x = gameMap->mapWidth - 1;
				}
				else {
					neighborPosition.x = -1;
				}
			}

			if ((y - 1) >= 0) {
				neighborPosition.y = y - 1;
			}
			else {
				if (gameMap->wrapVertical) {
					neighborPosition.y = gameMap->mapHeight - 1;
				}
				else {
					neighborPosition.y = -1;
				}
			}
		} break;
		default: /* some error? */ break;
	}

	return neighborPosition;
}

int32_t GetNeighborIndex(GameMap* gameMap, int32_t x, int32_t y, TileNeighbor direction) {
	// -1 means neighbor is an invalid map position (out of bounds)
	int32_t index = 0;

	ivec2 result = GetNeighborPosition(gameMap, x, y, direction);
	if (result.x == -1 || result.y == -1) {
		index = -1;
	}
	else {
		index = result.x + result.y * gameMap->mapWidth;
	}

	return index;
}

int32_t GetNeighborIndex(GameMap* gameMap, int32_t tileIndex, TileNeighbor direction) {
	// -1 means neighbor is an invalid map position (out of bounds)
	int32_t index = 0;

	ivec2 result = GetNeighborPosition(gameMap, tileIndex, direction);
	if (result.x == -1 || result.y == -1) { 
		index = -1;
	}
	else {
		index = result.x + result.y * gameMap->mapWidth;
	}

	return index;
}

void GenerateTerrain(GameMap* gameMap, PermanentResourceAllocator* allocator) {
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
			gameMap->tiles[x + y * gameMap->mapWidth].elevation = floorf(e*2.0f);
		}
	}

	for (int y = 0; y < gameMap->mapHeight; y++) {
		for (int x = 0; x < gameMap->mapWidth; x++) {
			f32 nx = (f32)x * 123.5f / (f32)gameMap->mapWidth - 0.5f;
			f32 ny = (f32)y * 123.5f / (f32)gameMap->mapHeight - 0.5f;
			f32 e = 0;
			f32 f = 0;
			f32 f_sum = 0;
			for (int n = 1; n <= 5; n++) {
				f = (1.0f / n);
				f_sum += f;
				e += f * noise(n * nx, n * ny);
			}
			e /= f_sum;

			if (e > 0.8f) {
				gameMap->tiles[x + y * gameMap->mapWidth].feature = TileFeatures::FOREST;
			}
			else {
				gameMap->tiles[x + y * gameMap->mapWidth].feature = TileFeatures::NONE;
			}
		}
	}

	gameMap->tiles[10 + 15 * gameMap->mapWidth].resources[0] = TileResource::SMALL_GAME;
	gameMap->tiles[11 + 14 * gameMap->mapWidth].resources[0] = TileResource::LARGE_GAME;
	gameMap->tiles[11 + 15 * gameMap->mapWidth].resources[0] = TileResource::SMALL_GAME;
	gameMap->tiles[11 + 16 * gameMap->mapWidth].resources[0] = TileResource::STONE;
	gameMap->tiles[10 + 16 * gameMap->mapWidth].resources[0] = TileResource::FRUITS;
	gameMap->tiles[9  + 16 * gameMap->mapWidth].resources[0] = TileResource::GRAINS;
	gameMap->tiles[9  + 15 * gameMap->mapWidth].resources[0] = TileResource::VEGETABLES;
	gameMap->tiles[9  + 14 * gameMap->mapWidth].resources[0] = TileResource::SMALL_GAME;
	gameMap->tiles[10 + 14 * gameMap->mapWidth].resources[0] = TileResource::STONE;
}

void GenerateTerrainMap(TerrainMap* t) {
	for (int y = 0; y < t->height; y++) {
		for (int x = 0; x < t->width; x++) {
			f32 nx = (f32)x / (f32)t->width - 0.5f;
			f32 ny = (f32)y / (f32)t->height - 0.5f;
			f32 e = 0;
			f32 f = 0;

			e = 1 * noise(nx, ny) + 0.5 * noise(2 * nx, 2 * ny) + 0.25 * noise(4 * nx, 4 * ny);
			f = 1 + 0.5 + 0.25;
			e = e / f;
			
			t->elevation[x + y * t->width] = powf(e * 1.03f, 2.0f) * 100;
		}
	}
}

void InitGameState(Memory* gameMemory, vec2 windowDimensions) {
	GameState* gameState = (GameState*)gameMemory->data;

	gameState->resourceAllocator = PermanentResourceAllocator(Megabytes(64));

	gameState->numEntities = 0;

	gameState->blackGuyHead = { Vec3(-1.0f, 0.0f, -1.0f), Vec3(1.0f, 1.0f, 1.0f), UpVec(), 0.0f };
	gameState->deerTest = { Vec3(-5.0f, 0.0f, -1.0f), Vec3(1.0f, 1.0f, 1.0f), UpVec(), 0.0f };
	gameState->bunnyTest = { Vec3(-3.0f, 0.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gameState->treeTest = { Vec3(-2.0f, 0.0f, -1.0f), Vec3(1.0f, 1.0f, 1.0f), UpVec(), 0.0f };

	gameState->cubes[0] = { Vec3(-1.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gameState->cubes[1] = { Vec3(-2.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gameState->cubes[2] = { Vec3(-3.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gameState->cubes[3] = { Vec3(-4.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gameState->cubes[4] = { Vec3(-5.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gameState->cubes[5] = { Vec3(-6.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };
	gameState->cubes[6] = { Vec3(-7.0f, 5.0f, -1.0f), Vec3(0.1f, 0.1f, 0.1f), UpVec(), 0.0f };

	gameState->quad = { Vec3(-5.0f, 3.0f, -3.0f), OneVec(), UpVec(), 0.0f };
	gameState->waterQuad = { Vec3(100.0f, 1.75f, 50.0f), Vec3(200.0f, 100.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), 90.0f };

	gameState->gameMap.wrapHorizontal = true;
	gameState->gameMap.wrapVertical = false;
	gameState->gameMap.ent = { Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), UpVec(), 0.0f };
	gameState->gameMap.mapWidth = 100;
	gameState->gameMap.mapHeight = 100;
	GenerateTerrain(&gameState->gameMap, &gameState->resourceAllocator);

	gameState->terrainMap.ent = { Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), UpVec(), 0.0f };
	gameState->terrainMap.width = 1000;
	gameState->terrainMap.height = 1000;
	gameState->terrainMap.elevation = (f32*)gameState->resourceAllocator.Allocate(sizeof(f32) * gameState->terrainMap.width * gameState->terrainMap.height);
	GenerateTerrainMap(&gameState->terrainMap);

	gameState->numEntities = 15;

	gameState->mainCamera.pos = Vec3(18.0f, 14.0f, 22.0f);
	gameState->mainCamera.dir = Vec3(1.0f, 0.0f, 0.0f);
	gameState->mainCamera.up = Vec3(0.0, 1.0, 0.0f);

	gameState->mainCamera.pitch = 0.0f;
	gameState->mainCamera.yaw = 0.0f;

	// gameState->mainCamera.view = LookAtMat(gameState->mainCamera.pos, gameState->mainCamera.target, gameState->mainCamera.up);
	gameState->mainCamera.view = LookAtMat(gameState->mainCamera.pos, gameState->mainCamera.dir, gameState->mainCamera.up);
	gameState->mainCamera.proj = PerspectiveMat(90.0f, windowDimensions.x / windowDimensions.y, 0.1f, 1000.0f);
}

void GameUpdate(Memory* gameMemory, Input* gameInput, f32 dt) {
	GameState* gameState = (GameState*)gameMemory->data;

	// ========================================================================
	// Camera Update

	Camera* camera = &gameState->mainCamera;

	const f32 cameraSpeed = 0.1f;
	const f32 rotateSpeed = 0.15f;
	
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
		cosf(DegToRad(camera->yaw)) * cosf(DegToRad(camera->pitch)),
		sinf(DegToRad(camera->pitch)),
		sinf(DegToRad(camera->yaw)) * cosf(DegToRad(camera->pitch))
	);
	camera->dir = NormVec(dir);

	gameState->mainCamera.view = LookAtMat(gameState->mainCamera.pos, AddVec(gameState->mainCamera.pos, gameState->mainCamera.dir), gameState->mainCamera.up);

	char debug_str[256];
	snprintf(debug_str, 256, "Camera: (%f, %f, %f)\n", camera->pos.x, camera->pos.y, camera->pos.z);
	//DebugPrint(debug_str);

	// end Camera Update
	// ========================================================================


}