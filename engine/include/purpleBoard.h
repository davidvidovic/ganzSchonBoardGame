#ifndef PURPLE_BOARD_H
#define PURPLE_BOARD_H

#include "board.h"

namespace engine {

class PurpleBoard : NumericBoard {
    public:
    PurpleBoard() : NumericBoard() {
        board_values = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        board_multipliers = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
        board_state = {false, false, false, false, false, false, false, false, false, false, false};
        board_bonus_map = {
            GameBonus::NONE, 
            GameBonus::NONE, 
            GameBonus::REROLL, 
            GameBonus::GREEN_X, 
            GameBonus::ORANGE_ZERO, 
            GameBonus::FOX, 
            GameBonus::PLUS_ONE, 
            GameBonus::BLUE_X, 
            GameBonus::REROLL, 
            GameBonus::YELLOW_X, 
            GameBonus::PLUS_ONE
        };
    }

    std::vector<int> isPlayable(int diceValue) override {
        if(currentIndex >= PURPLE_BOARD_LENGTH) return {};
        if(currentIndex < 1) return {currentIndex};

        if(diceValue > board_values[currentIndex-1])
            return {currentIndex};
        else
            return {};
    }
    
    void play(int diceValue) override {
        score += diceValue;
        board_values[currentIndex] = diceValue;
        board_state[currentIndex] = true;
        currentIndex++;
    }

    std::vector<int> getBoardValues() {
        return board_values;
    }

    std::string getBoardAsString() {
        std::string board;
        for(const auto& v : board_state) {
            if(v) {
                board.append(std::to_string(board_values[currentIndex]));
            }
            else {
                board.append("F");
            }
        }

        return board;
    }
};


}

#endif