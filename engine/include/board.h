#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include "includes.h"

namespace engine {

class Board {
    protected:
    int currentIndex = 0;
    int score = 0;
    std::vector<GameBonus::GameBonus> board_bonus_map;

    public:
    virtual std::vector<int> isPlayable(int diceValue) = 0;
    virtual void play(int) = 0;
};

/* 
* Lets split boards into two categories: boolean (checkable) and numeric 
* yellow, blue, and green boards are boolean
* orange and purple boards are numeric
*/

class BooleanBoard : public Board {
    protected:
    std::vector<int> board_box_requirements;
    std::vector<bool> board_state; 
    virtual std::vector<bool> getBoardState() = 0;
};

class NumericBoard : public Board {
    protected:
    std::vector<int> board_multipliers;
    std::vector<int> board_values;
    virtual std::vector<int> getBoardValues() = 0;
};

}

#endif