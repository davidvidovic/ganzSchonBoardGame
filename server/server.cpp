#include "crow.h"
#include "../engine/include/gameEngine.h"

engine::GameEngine game;

int main()
{
    crow::SimpleApp app;
    crow::mustache::set_global_base("server/templates");

    CROW_ROUTE(app, "/")([](){
        auto page = crow::mustache::load_text("home.html");
        return page;
    });

    CROW_ROUTE(app, "/setName").methods("POST"_method)
    ([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400);

        std::string playerName = body["name"].s();
        engine::Player p(playerName);
        bool playerAdded = game.addPlayer(p);

        // DEBUG info
        // TODO: remove
        auto players = game.getPlayers();
        for(auto& p : players) {
            std::cout << p.getName() << '\n';
        }

        if(!playerAdded) {
            return crow::response(500);
        }

        return crow::response(200);
    });

    CROW_ROUTE(app, "/roll")
    ([](){
        auto page = crow::mustache::load_text("board.html");
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