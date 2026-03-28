#ifndef DICE_H
#define DICE_H

/* A simple header file for Dice object */
/* Dice is an object, and not a global variable, in order to keep thing object oriented */

#include <cstdint>
#include <cstdlib>
#include <random>
#include <chrono>
#include "includes.h"
#include "gameColor.h"

namespace engine {

class Dice {
    private:
    int value;
    GameColor::GameColor color;
    DiceState::DiceState state;

    std::minstd_rand randomGenerator;
    std::uniform_int_distribution<> distribution;

    public:
    Dice(GameColor::GameColor color);

    int getValue();
    GameColor::GameColor getColor();
    void setState(DiceState::DiceState s);
    DiceState::DiceState getState();

    void rollDice();

    friend bool operator== (const Dice& c1, const Dice& c2);
    friend bool operator!= (const Dice& c1, const Dice& c2);

    friend bool operator< (const Dice& c1, const Dice& c2);
    friend bool operator> (const Dice& c1, const Dice& c2);

    friend bool operator<= (const Dice& c1, const Dice& c2);
    friend bool operator>= (const Dice& c1, const Dice& c2);
};

}

#endif