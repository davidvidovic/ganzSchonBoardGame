#ifndef INCLUDES_H
#define INCLUDES_H

namespace engine {

namespace GameColor{
typedef enum GameColor { WHITE, BLUE, YELLOW, PURPLE, GREEN, ORANGE } GameColor;
}

namespace GameBonus{
typedef enum GameBonus {NONE, ORANGE_ZERO, BLUE_X, FOX, REROLL, PLUS_ONE, PURPLE_SIX, GREEN_X, YELLOW_X, YELLOW_20, YELLOW_16, YELLOW_12, YELLOW_22} GameBonus;
}

const int YELLOW_BOARD_LENGTH = 16;
const int BLUE_BOARD_LENGTH = 12;
const int GREEN_BOARD_LENGTH = 11;
const int ORANGE_BOARD_LENGTH = 11;
const int PURPLE_BOARD_LENGTH = 11;

}

#endif