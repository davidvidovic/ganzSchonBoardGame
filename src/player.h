#ifndef PLAYER_H
#define PLAYER_H

#include "greenBoard.h"
#include "yellowBoard.h"
#include "blueBoard.h"
#include "orangeBoard.h"
#include "purpleBoard.h"

namespace engine {

class Player {
    public:
    bool isTurn{false};
    int score{0};
    GreenBoard greenBoard;
    YellowBoard yellowBoard;
    BlueBoard blueBoard;
    OrangeBoard orangeBoard;
    PurpleBoard purpleBoard;
    // Bonuses: plus ones, re-rolls

};


}

#endif