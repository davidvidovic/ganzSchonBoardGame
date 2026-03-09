#ifndef YELLOW_BOARD_H
#define YELLOW_BOARD_H

#include "board.h"

namespace engine {

class YellowBoard : BooleanBoard {
    public:
    YellowBoard() : BooleanBoard() {
        board_box_requirements = {
            1, 0, 4, 3, 
            5, 4, 6, 0, 
            0, 5, 2, 1, 
            2, 3, 0, 6
        };

        board_state = {
            false, true, false, false, 
            false, false, false, true, 
            true, false, false, false, 
            false, false, true, false
        };

        board_bonus_map = {
            // First rows
            GameBonus::ORANGE_ZERO,
            GameBonus::YELLOW_20,
            GameBonus::BLUE_X,
            GameBonus::YELLOW_16,
            // Columns
            GameBonus::YELLOW_12,
            GameBonus::PURPLE_SIX,
            GameBonus::PLUS_ONE,
            GameBonus::REROLL,
            // Positive diagonal
            GameBonus::YELLOW_22,
            // Negative diagonal
            GameBonus::FOX
        };
    }

    std::vector<int> isPlayable(int diceValue) override {
        std::vector<int> playableCoordinates{};
        for(size_t i = 0; i < board_box_requirements.size(); i++)
        {
            if(diceValue == board_box_requirements[i])
                playableCoordinates.push_back(i);
        }

        return playableCoordinates;
    }
    
    void play(int coordinate) override {
        std::vector<int> playableCoorindates = isPlayable(board_box_requirements[coordinate]);

        bool playable = false;
        for(auto& c : playableCoorindates)
        {
            if(c == coordinate)
            {
                playable = true;
                break;
            }
        }
        if(!playable) return;

        board_state[coordinate] = true;
        // Bonuses
    }
};


}

#endif