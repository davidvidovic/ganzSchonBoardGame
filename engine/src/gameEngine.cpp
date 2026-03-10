/* This is the game engine. */

#include "../include/gameEngine.h"
#include <iostream>

namespace engine
{

long long GameEngine::gameId = 0;

void GameEngine::addPlayer(Player& player) {
    if(static_cast<int>(players.size()) < 4) {
        players.push_back(player);
    }
}

void GameEngine::removePlayer(Player& player) {
    for(size_t i = 0; i < players.size(); i++) {
        if(player == players[i]) {
            players.erase(players.begin() + i);
            return;
        }
    }
}

const std::vector<Player>& GameEngine::getPlayers() const {
    return players;
}

Dices& GameEngine::getDices() {
    return dices;
}

void GameEngine::rollDices() {
    dices.rollDices();
}

void GameEngine::sortDices() {
    dices.sortDices();
}

}