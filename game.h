#ifndef GAME_H
#define GAME_H

#include "universal.h"

struct GameState {
	ivec2 position;
};

void InitGameState(Memory* gameMemory);
void GameUpdate(Memory* gameMemory, Input* gameInput);

#endif
