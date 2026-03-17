#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <string>
#include <unordered_map>
#include "dices.h"
#include "player.h"
#include "includes.h"

namespace engine
{

class GameEngine {
    private:
    static long long gameId;
    std::vector<Player> players;
    Dices dices;
    int round{0};
    std::vector<GameBonus::GameBonus> roundBonus{GameBonus::REROLL, GameBonus::PLUS_ONE, GameBonus::REROLL, GameBonus::UNIVERSAL_X_OR_6};

    public:
    bool addPlayer(Player& player);
    bool addPlayer(std::string_view player);
    void removePlayer(Player& player);
    const std::vector<Player>& getPlayers() const;
    Dices& getDices();
    std::unordered_map<GameColor::GameColor, int> getDiceValues();
    void rollDices();
    void sortDices();
    void startGame();
    void assignRoundBonus();
    Player* findPlayerById(int playerId);
};

}

#endif