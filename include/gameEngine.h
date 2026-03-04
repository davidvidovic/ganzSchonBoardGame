#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include "dices.h"
#include "player.h"

namespace engine
{

class GameEngine {
    private:
    std::vector<Player> players;
    Dices dices;

    public:
    void addPlayer(Player& player);
    void removePlayer(Player& player);
};

}

#endif