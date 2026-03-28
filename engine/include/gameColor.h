#ifndef GAME_COLOR_H
#define GAME_COLOR_H

namespace engine {

namespace GameColor{
    typedef enum GameColor { WHITE, BLUE, YELLOW, PURPLE, GREEN, ORANGE } GameColor;

    inline GameColor stringToGameColor(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);

        if (str == "WHITE") return WHITE;
        if (str == "BLUE") return BLUE;
        if (str == "YELLOW") return YELLOW;
        if (str == "GREEN") return GREEN;
        if (str == "PURPLE") return PURPLE;
        if (str == "ORANGE") return ORANGE;

        throw std::invalid_argument("Invalid GameColor string: " + str);
    }

    inline std::string gameColorToString(GameColor color) {
    switch(color)
    {
        case GameColor::WHITE:  return "white";
        case GameColor::BLUE:   return "blue";
        case GameColor::YELLOW: return "yellow";
        case GameColor::PURPLE: return "purple";
        case GameColor::GREEN:  return "green";
        case GameColor::ORANGE: return "orange";
        default: return "unknown";
    }
}

}
}

#endif