#include "crow.h"
#include "../engine/include/gameEngine.h"

engine::GameEngine game;

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app,"/")
    ([](){
        return "Welcome!\nLets play!";
    });

    CROW_ROUTE(app,"/rollDice").methods("GET"_method)
    ([](){
        game.getDices().rollDices();
        game.getDices().sortDices();

        crow::json::wvalue dices;
        dices["blue"] = game.getDices().getDice(engine::GameColor::BLUE).getValue();
        dices["yellow"] = game.getDices().getDice(engine::GameColor::YELLOW).getValue();
        dices["orange"] = game.getDices().getDice(engine::GameColor::ORANGE).getValue();
        dices["white"] = game.getDices().getDice(engine::GameColor::WHITE).getValue();
        dices["green"] = game.getDices().getDice(engine::GameColor::GREEN).getValue();
        dices["purple"] = game.getDices().getDice(engine::GameColor::PURPLE).getValue();

        return dices;
    });

    app.port(18080).multithreaded().run();
}