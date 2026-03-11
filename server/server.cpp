#include "crow.h"
#include "../engine/include/gameEngine.h"
#include <unordered_map>
#include <random>

engine::GameEngine game;
std::unordered_map<std::string, const engine::Player*> sessions;

std::string generateSessionId()
{
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::string result;
    result.resize(32);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    for (auto& c : result)
        c = charset[dist(gen)];

    return result;
}

std::string getSessionId(const crow::request& req)
{
    auto cookie = req.get_header_value("Cookie");

    std::string key = "session_id=";
    auto pos = cookie.find(key);

    if(pos == std::string::npos)
        return "";

    pos += key.size();

    auto end = cookie.find(";", pos);

    if(end == std::string::npos)
        end = cookie.size();

    return cookie.substr(pos, end - pos);
}

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
        if (!body) 
            return crow::response(400);

        std::string playerName = body["name"].s();

        engine::Player p(playerName);

        bool playerAdded = game.addPlayer(p);
        if(!playerAdded)
            return crow::response(500);

        auto& players = game.getPlayers();
        const engine::Player* player = &players.back();

        std::string sessionId = generateSessionId();
        sessions[sessionId] = player;

        crow::response res(200);
        res.add_header("Set-Cookie", "session_id=" + sessionId + "; Path=/");

        return res;
    });

    CROW_ROUTE(app, "/board")
    ([&](const crow::request& req){
        std::string sessionId = getSessionId(req);

        for(auto& s : sessions) {
            std::cout << s.first << '\n';
        }

        if(sessions.find(sessionId) == sessions.end())
            return crow::response(403);

        auto page = crow::mustache::load_text("board.html");
        return crow::response(page);
    });

    CROW_ROUTE(app, "/rollDice").methods("POST"_method)
    ([&](const crow::request& req){

        std::string sessionId = getSessionId(req);

        if(sessions.find(sessionId) == sessions.end())
            return crow::response(403);

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

        return crow::response(result);
    });
    

    app.port(18080).multithreaded().run();
}