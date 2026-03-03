#ifndef GREEN_BOARD_H
#define GREEN_BOARD_H

#include "board.h"

namespace engine {

class GreenBoard : BooleanBoard {
    public:
    GreenBoard() : BooleanBoard() {
        board_box_requirements = {2, 4, 3, 5, 1, 4, 2, 5, 1, 3, 6};
        board_bonus_map = {
            GameBonus::NONE, 
            GameBonus::NONE, 
            GameBonus::REROLL, 
            GameBonus::NONE, 
            GameBonus::YELLOW_X, 
            GameBonus::PLUS_ONE, 
            GameBonus::NONE, 
            GameBonus::BLUE_X, 
            GameBonus::FOX, 
            GameBonus::NONE, 
            GameBonus::NONE
        };
        board_state = {false, false, false, false, false, false, false, false, false, false, false};
    }

    std::vector<int> isPlayable(int diceValue) override {
        if(currentIndex >= GREEN_BOARD_LENGTH) return {};

        if(diceValue >= board_box_requirements[currentIndex])
            return {currentIndex+1};
        else
            return {};
    }
    
    void play(int diceValue) override {
        score = (currentIndex+1)*(currentIndex+2)/2;
        board_state[currentIndex] = true;
        currentIndex++;
    }
};


}

#endif