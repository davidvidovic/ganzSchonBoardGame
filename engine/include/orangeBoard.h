#ifndef ORANGE_BOARD_H
#define ORANGE_BOARD_H

#include "board.h"

namespace engine {

class OrangeBoard : NumericBoard {
    public:
    OrangeBoard() : NumericBoard() {
        board_values = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        board_multipliers = {-1, 1, 2, 3, -1, 1, 2, 3, -1, 1, 4};
        board_bonus_map = {
            GameBonus::NONE, 
            GameBonus::NONE, 
            GameBonus::PLUS_ONE, 
            GameBonus::NONE, 
            GameBonus::PURPLE_SIX, 
            GameBonus::REROLL, 
            GameBonus::FOX, 
            GameBonus::NONE, 
            GameBonus::NONE, 
            GameBonus::GREEN_X, 
            GameBonus::NONE
        };
    }

    std::vector<int> isPlayable([[maybe_unused]] int diceValue) override {
        if(currentIndex >= ORANGE_BOARD_LENGTH) return {};

        return {currentIndex+1};
    }
    
    void play([[maybe_unused]] int diceValue) override {
        int new_value = diceValue * board_multipliers[currentIndex];
        score += new_value;
        board_values[currentIndex] = new_value;
        currentIndex++;
    }
};


}

#endif