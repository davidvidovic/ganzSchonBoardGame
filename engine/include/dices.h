#ifndef DICES_H
#define DICES_H

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>

#include "dice.h"
#include "includes.h"
#include "gameColor.h"

namespace engine {

class Dices {
    private:
    std::vector<Dice> dices;

    public:
    Dices();
    void rollDices();
    void sortDices();
    const std::vector<Dice> getDices() const;
    Dice& getDice(GameColor::GameColor color);
};

}

#endif