#ifndef DICES_H
#define DICES_H

#include <vector>
#include <algorithm>
#include <cstdlib>
#include "dice.h"
#include "includes.h"

namespace engine {

class Dices {
    private:
    std::vector<Dice> dices;

    public:
    Dices();
    void rollDices();
    void sortDices();
    std::vector<Dice> getDices();
    Dice getDice(int color);
};

}

#endif