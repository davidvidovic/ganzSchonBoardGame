#include "../include/dice.h"

namespace engine {

Dice::Dice(GameColor::GameColor color) : color{color}, state{DiceState::AVAILABLE} {
    randomGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

int Dice::getValue() { 
    return value; 
}

GameColor::GameColor Dice::getColor() { 
    return color; 
}

DiceState::DiceState Dice::getState() { 
    return state; 
}

void Dice::setState(DiceState::DiceState s) { 
    state = s; 
}

void Dice::rollDice() {
    value = randomGenerator() % 6 + 1;
}

inline bool operator== (const Dice& c1, const Dice& c2)
{
    return c1.value == c2.value;
}

bool operator!= (const Dice& c1, const Dice& c2)
{
    return c1.value != c2.value;
}

bool operator> (const Dice& c1, const Dice& c2)
{
    return c1.value > c2.value;
}

bool operator< (const Dice& c1, const Dice& c2)
{
    return c1.value < c2.value;
}

bool operator<= (const Dice& c1, const Dice& c2)
{
    return c1.value <= c2.value;
}

bool operator>= (const Dice& c1, const Dice& c2)
{
    return c1.value >= c2.value;
}


}