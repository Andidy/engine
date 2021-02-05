#include "game.h"

void InitGameState(Memory* gameMemory) {
	GameState* gameState = (GameState*)gameMemory->data;

	gameState->position = IVec2(0, 0);
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