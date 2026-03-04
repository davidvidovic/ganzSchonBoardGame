#include <iostream>
#include "../include/gameEngine.h"

int main() {
    // Create game lobby and a game
    engine::GameEngine game;
    
    // Create 4 players
    engine::Player p1("David");
    engine::Player p2("Gorana");
    engine::Player p3("Elza");
    engine::Player p4("GogyDogy");

    game.addPlayer(p1);
    game.addPlayer(p2);
    game.addPlayer(p3);
    game.addPlayer(p4);

    std::cout << p1 << '\n';
    std::cout << p2 << '\n';
    std::cout << p3 << '\n';
    std::cout << p4 << '\n';
    
    // Create game mechanics, dice rolling, and player decisions
    return 0;
}