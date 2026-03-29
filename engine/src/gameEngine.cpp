/* This is the game engine. */

#include "../include/gameEngine.h"

namespace engine
{

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
    players[0].setMovesLeft(3);
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

Player* GameEngine::getPlayerOnTurn() {
    for (auto& p : players) {
        if (p.getIsTurn() == true)
            return &p;
    }
    return nullptr;
}

crow::json::wvalue GameEngine::getPlayersTurn() {
    crow::json::wvalue message;

    message["player4"] = players[0].getIsTurn() ? "1" : "0";
    message["player3"] = players[1].getIsTurn() ? "1" : "0";
    message["player2"] = players[2].getIsTurn() ? "1" : "0";
    message["player1"] = players[3].getIsTurn() ? "1" : "0";

    return message;
}

bool GameEngine::setNextPlayerTurn() {
    for (size_t i = 0; i < players.size(); i++) {
        if (players[i].getIsTurn() == true) {
            if(i == players.size() - 1) {
                return false; // new round must start
            }

            players[i].setIsTurn(false);
            players[i+1].setIsTurn(true);
            return true;
        } 
    }

    return false;
}

void GameEngine::setDiceColorLastPlayed(GameColor::GameColor color) {
    diceColorLastPlayed = color;
}

GameColor::GameColor GameEngine::getDiceColorLastPlayed() {
    return diceColorLastPlayed;
}

void GameEngine::setDiceIndexLastPlayed(int index) {
    diceIndexLastPlayed = index;
}

int GameEngine::getDiceIndexLastPlayed() {
    return diceIndexLastPlayed;
}

crow::json::wvalue GameEngine::getGameState() {
    crow::json::wvalue message;

    message["round"] = round;
    message["roundSate"] = RoundInfo::roundStateToString(roundState);

    return message;
}

bool GameEngine::setNextRound() {
    round++;
    if(round > 4) {
        return false;
    }

    return true;
}

}