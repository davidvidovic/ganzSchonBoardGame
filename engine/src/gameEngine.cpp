/* This is the game engine. */

#include "../include/gameEngine.h"

namespace engine
{

long long GameEngine::gameId = 0;

bool GameEngine::addPlayer(Player& player) {
    if(static_cast<int>(players.size()) < 4) {
        players.push_back(player);
        return true;
    }

    return false;
}

bool GameEngine::addPlayer(std::string_view player_name) {
    Player p(player_name);
    return addPlayer(p);
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