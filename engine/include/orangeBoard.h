#ifndef ORANGE_BOARD_H
#define ORANGE_BOARD_H

#include "board.h"

namespace engine {

class OrangeBoard : NumericBoard {
    public:
    OrangeBoard() : NumericBoard() {
        board_values = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        board_multipliers = {-1, 1, 2, 3, -1, 1, 2, 3, -1, 1, 4};
        board_state = {false, false, false, false, false, false, false, false, false, false, false};
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

    std::vector<int> isPlayable([[maybe_unused]] int diceValue) const override {
        if(currentIndex >= ORANGE_BOARD_LENGTH) return {};

        return {currentIndex};
    }
    
    void play([[maybe_unused]] int diceValue) override {
        int new_value = diceValue * board_multipliers[currentIndex];
        score += new_value;
        board_values[currentIndex] = diceValue;
        board_state[currentIndex] = true;
        currentIndex++;
    }

    std::vector<int> getBoardValues() {
        return board_values;
    }

    std::string getBoardAsString() {
        std::string board;
        int it = 0;
        for(const auto& v : board_state) {
            if(v) {
                board.append(std::to_string(board_values[it]));
            }
            else {
                board.append("F");
            }
            ++it;
        }

        return board;
    }
};


}

#endif