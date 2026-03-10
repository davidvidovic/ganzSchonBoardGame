#include "../include/dices.h"

namespace engine {

Dices::Dices() : dices{
    Dice(GameColor::WHITE),
    Dice(GameColor::BLUE),
    Dice(GameColor::YELLOW),
    Dice(GameColor::GREEN),
    Dice(GameColor::PURPLE),
    Dice(GameColor::ORANGE)
} {}

const std::vector<Dice> Dices::getDices() const {
    return dices;
}

void Dices::rollDices() {
    for(auto& dice : dices) {
        if(!dice.getLocked())
        {
            dice.rollDice();
        }
    }
}

void Dices::sortDices() {
    std::sort(dices.begin(), dices.end());
}

Dice& Dices::getDice(int color) {
    for(auto& dice : dices) {
        if(dice.getColor() == color) {
            return dice;
        }
    }

    throw std::runtime_error("Dice not found");
}

}