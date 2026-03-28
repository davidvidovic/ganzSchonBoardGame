#include "../include/dice.h"

namespace engine {

Dice::Dice(GameColor::GameColor color) : color{ color } {
    locked = false;
    randomGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

int Dice::getValue() { 
    return value; 
}

GameColor::GameColor Dice::getColor() { 
    return color; 
}

uint8_t Dice::getLocked() { 
    return locked; 
}

void Dice::setLocked(bool lockedValue) { 
    locked = lockedValue; 
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