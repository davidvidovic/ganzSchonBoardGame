#include "../include/dices.h"

namespace engine {

Dices::Dices() : 
    dices{
        Dice(GameColor::WHITE),
        Dice(GameColor::BLUE),
        Dice(GameColor::YELLOW),
        Dice(GameColor::GREEN),
        Dice(GameColor::PURPLE),
        Dice(GameColor::ORANGE)
    }
 {}

std::vector<Dice>& Dices::getDices() {
    return dices;
}

void Dices::rollDices() {
    for(auto& dice : dices) {
        if(dice.getState() == DiceState::AVAILABLE)
        {
            dice.rollDice();
        }
    }
}

void Dices::sortDices() {
    auto it = std::partition(dices.begin(), dices.end(),
        [](Dice& d) {
            return d.getState() != DiceState::AVAILABLE;
        });

    std::sort(it, dices.end());
}

Dice& Dices::getDice(GameColor::GameColor color) {
    for(auto& dice : dices) {
        if(dice.getColor() == color) {
            return dice;
        }
    }

    throw std::runtime_error("Dice not found");
}

}