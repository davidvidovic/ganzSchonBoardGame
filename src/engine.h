#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include "dices.h"
#include "player.h"

namespace engine
{

class GameEngine {
    private:
    Player player; // should this be a vector, or Players?
    Dices dices;
    // playerSequence; ? 
    // round (1, 2, 3, 4)

    public:
    GameEngine();
};

}

#endif