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

std::unordered_map<GameColor::GameColor, int> GameEngine::getDiceValues() {
    std::unordered_map<GameColor::GameColor, int> values;
    auto allDices = dices.getDices();
    for(auto& d : allDices) {
        values[d.getColor()] = d.getValue();
    }

    return values;
}

void GameEngine::rollDices() {
    dices.rollDices();
}

void GameEngine::sortDices() {
    dices.sortDices();
}

void GameEngine::startGame() {
    players[0].setIsTurn(true);
}

void GameEngine::assignRoundBonus() {
    for(auto& p : players) {
        p.setBonuses(roundBonus[round]);
    }
}

Player* GameEngine::findPlayerById(int playerId)
{
    for (auto& p : players) {
        if (p.getId() == playerId)
            return &p;
    }
    return nullptr;
}

}