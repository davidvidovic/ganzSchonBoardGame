#ifndef BLUE_BOARD_H
#define BLUE_BOARD_H

#include "board.h"

namespace engine {

class BlueBoard : BooleanBoard {
    public:
    BlueBoard() : BooleanBoard() {
        board_bonus_map = {
            // Rows
            GameBonus::FOX,
            GameBonus::GREEN_X,
            GameBonus::PURPLE_SIX,
            // Columns
            GameBonus::ORANGE_ZERO,
            GameBonus::BLUE_X,
            GameBonus::YELLOW_X,
            GameBonus::PLUS_ONE
        };

        board_box_requirements = {2, 5, 7, 10, 3, 6, 8, 11, 4, 7, 9, 12};
        board_state = {
            false, false, false, false,
            false, false, false, false,
            false, false, false, false
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