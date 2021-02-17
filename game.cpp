#include "game.h"

void InitGameState(Memory* gameMemory, vec2 windowDimensions) {
	GameState* gameState = (GameState*)gameMemory->data;

	gameState->position = IVec2(0, 0);
	
	gameState->mainCamera.pos = Vec3(0.0f, 0.0f, -5.0f);
	gameState->mainCamera.target = ZeroVec();
	gameState->mainCamera.up = UpVec();
	gameState->mainCamera.view = LookAtMat(gameState->mainCamera.pos, gameState->mainCamera.target, gameState->mainCamera.up);
	gameState->mainCamera.proj = PerspectiveMat(90.0f, windowDimensions.x / windowDimensions.y, 0.0001f, 1000.0f);
}

void GameUpdate(Memory* gameMemory, Input* gameInput) {
	GameState* gameState = (GameState*)gameMemory->data;

	if (keyReleased(gameInput->keyboard.up)) {
		gameState->position.y += 1;
	}
	else if (keyReleased(gameInput->keyboard.down)) {
		gameState->position.y -= 1;
	}
	else if (keyReleased(gameInput->keyboard.left)) {
		gameState->position.x -= 1;
	}
	else if (keyReleased(gameInput->keyboard.right)) {
		gameState->position.x += 1;
	}
}