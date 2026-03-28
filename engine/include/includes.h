#ifndef INCLUDES_H
#define INCLUDES_H

namespace engine {

namespace GameBonus{
typedef enum GameBonus {NONE, ORANGE_ZERO, BLUE_X, FOX, REROLL, PLUS_ONE, PURPLE_SIX, GREEN_X, YELLOW_X, YELLOW_20, YELLOW_16, YELLOW_12, YELLOW_22, UNIVERSAL_X_OR_6} GameBonus;
}

const int YELLOW_BOARD_LENGTH = 16;
const int BLUE_BOARD_LENGTH = 12;
const int GREEN_BOARD_LENGTH = 11;
const int ORANGE_BOARD_LENGTH = 11;
const int PURPLE_BOARD_LENGTH = 11;

namespace DiceState{
typedef enum DiceState {LOCKED, AVAILABLE, PLAYED} DiceState;

inline std::string diceStateToString(DiceState state) {
    switch(state)
    {
        case LOCKED:  return "locked";
        case AVAILABLE:   return "available";
        case PLAYED: return "played";
        default: return "unknown";
    }
}
}

}

#endif