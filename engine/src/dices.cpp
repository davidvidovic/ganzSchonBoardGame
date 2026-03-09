#include "../include/dices.h"

namespace engine {

Dices::Dices() {
    dices = {
        Dice(GameColor::WHITE),
        Dice(GameColor::BLUE),
        Dice(GameColor::YELLOW),
        Dice(GameColor::GREEN), 
        Dice(GameColor::PURPLE),
        Dice(GameColor::ORANGE)
    };
}

std::vector<Dice> Dices::getDices() {
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

}