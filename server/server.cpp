#include "crow.h"
#include "../engine/include/gameEngine.h"

engine::GameEngine game;

int main()
{
    crow::SimpleApp app;
    crow::mustache::set_global_base("server/templates");

    CROW_ROUTE(app,"/")
    ([](){
        auto page = crow::mustache::load_text("home.html");
        return page;
    });

    CROW_ROUTE(app, "/roll")
    ([](){
        auto page = crow::mustache::load_text("rollDices.html");
        return page;
    });

CROW_ROUTE(app, "/rollDice").methods("POST"_method)
([](){
    game.getDices().rollDices();
    game.getDices().sortDices();

    auto dices = game.getDices().getDices();

    crow::json::wvalue result = crow::json::wvalue::list();

    for (size_t i = 0; i < dices.size(); ++i) {
        crow::json::wvalue obj;
        obj["color"] = dices[i].gameColorToString(dices[i].getColor());
        obj["value"] = dices[i].getValue();

        result[i] = std::move(obj);
    }

    return result;
});

    app.port(18080).multithreaded().run();
}