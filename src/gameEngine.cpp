/* This is the game engine. */

#include "../include/gameEngine.h"
#include <iostream>

namespace engine
{

void GameEngine::addPlayer(Player& player) {
    if(players.size() < 4) {
        players.push_back(player);
    }
}

void GameEngine::removePlayer(Player& player) {
    for(int i = 0; i < players.size(); i++) {
        if(player == players[i]) {
            players.erase(players.begin() + i);
            return;
        }
    }
}


}