#include "crow.h"
#include "../engine/include/gameEngine.h"
#include "../engine/include/gameColor.h"
#include <unordered_map>
#include <random>
#include <mutex>
#include <algorithm>
#include <vector>

engine::GameEngine game;
std::unordered_map<std::string, int> sessions;
std::vector<crow::websocket::connection*> connections;
std::mutex connectionsMutex;
std::mutex sessionsMutex;

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

    CROW_WEBSOCKET_ROUTE(app, "/ws")
    .onopen([&](crow::websocket::connection& conn){
        std::lock_guard<std::mutex> lock(connectionsMutex);
        connections.push_back(&conn);
    })
    .onclose([&](crow::websocket::connection& conn, const std::string&, uint16_t){
        std::lock_guard<std::mutex> lock(connectionsMutex);
        connections.erase(
            std::remove(connections.begin(), connections.end(), &conn),
            connections.end()
        );
    });
    
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

        auto players = game.getPlayers();
        int playerId = players.back().getId();

        std::string sessionId = generateSessionId();
        {
            std::lock_guard<std::mutex> lock(sessionsMutex);
            sessions[sessionId] = playerId;
        }

        crow::response res(200);
        res.add_header("Set-Cookie", "session_id=" + sessionId + "; Path=/");

        return res;
    });

    CROW_ROUTE(app, "/playerJoined").methods("POST"_method)
    ([](const crow::request& req){
        std::string sessionId = getSessionId(req);
        {
            std::lock_guard<std::mutex> lock(sessionsMutex);
            if (sessions.find(sessionId) == sessions.end())
                return crow::response(403);
        }

        auto& players = game.getPlayers();
        crow::json::wvalue namesList = crow::json::wvalue::list();
        size_t i = 0;
        for (auto& p : players) {
            crow::json::wvalue obj;
            obj["name"] = std::string(p.getName());
            namesList[i++] = std::move(obj);
        }

        crow::json::wvalue message;
        message["type"] = "playerJoined";
        message["numberOfPlayers"] = players.size();
        message["players"] = std::move(namesList);
        if(players.size() == 4) {
            message["startGame"] = "true";
            game.startGame();
        }
        else {
            message["startGame"] = "false";
        }

        std::string msg = message.dump();

        {
            std::lock_guard<std::mutex> lock(connectionsMutex);
            for (auto* c : connections)
                c->send_text(msg);
        }

        return crow::response(200);
    });

    CROW_ROUTE(app, "/playerLeft").methods("POST"_method)
    ([](const crow::request& req){
        std::string sessionId = getSessionId(req);
        {
            std::lock_guard<std::mutex> lock(sessionsMutex);
            if (sessions.find(sessionId) == sessions.end())
                return crow::response(403);
        }
        std::cout << req.raw_url << std::endl;

        // I think the game should end here

        return crow::response(200);
    });

    CROW_ROUTE(app, "/board")
    ([&](const crow::request& req){
        std::string sessionId = getSessionId(req);

        {
            std::lock_guard<std::mutex> lock(sessionsMutex);
            if (sessions.find(sessionId) == sessions.end())
                return crow::response(403);
        }

        auto page = crow::mustache::load_text("board.html");
        return crow::response(page);
    });

    CROW_ROUTE(app, "/rollDice").methods("POST"_method)
    ([&](const crow::request& req){

        std::string sessionId = getSessionId(req);
        {
            std::lock_guard<std::mutex> lock(sessionsMutex);
            if (sessions.find(sessionId) == sessions.end())
                return crow::response(403);
        }

        int playerId;
        {
            std::lock_guard<std::mutex> lock(sessionsMutex);
            auto it = sessions.find(sessionId);
            if (it == sessions.end())
                return crow::response(403);

            playerId = it->second;
        }

        engine::Player* player = game.findPlayerById(playerId);

        game.getDices().rollDices();
        game.getDices().sortDices();

        auto dices = game.getDices().getDices();

        crow::json::wvalue diceList = crow::json::wvalue::list();

        for (size_t i = 0; i < dices.size(); ++i) {
            crow::json::wvalue obj;
            obj["color"] = dices[i].gameColorToString(dices[i].getColor());
            obj["value"] = dices[i].getValue();
            obj["isLocked"] = dices[i].getLocked();
            diceList[i] = std::move(obj);
        }

        crow::json::wvalue message;
        message["type"] = "diceRolled";
        message["dice"] = std::move(diceList);


        // Update playable fields for this player
        message["playebleFields"] = player->getPlayableFieldsAsJSON(game.getDiceValues());

        std::string msg = message.dump();

        {
            std::lock_guard<std::mutex> lock(connectionsMutex);
            for (auto* c : connections)
                c->send_text(msg);
        }

        return crow::response(200);
    });


    CROW_ROUTE(app, "/updateBoard").methods("POST"_method)
    ([&](const crow::request& req){
        std::string sessionId = getSessionId(req);

        {
            std::lock_guard<std::mutex> lock(sessionsMutex);
            if (sessions.find(sessionId) == sessions.end())
                return crow::response(403);
        }

        int playerId;
        {
            std::lock_guard<std::mutex> lock(sessionsMutex);
            auto it = sessions.find(sessionId);
            if (it == sessions.end())
                return crow::response(403);

            playerId = it->second;
        }

        engine::Player* player = game.findPlayerById(playerId);
        if (!player)
            return crow::response(404);

        crow::json::wvalue message;
        message["type"] = "updateBoard";
        message["state"] = player->getBoardAsJSON();

        std::string msg = message.dump();

        {
            std::lock_guard<std::mutex> lock(connectionsMutex);
            for (auto* c : connections) {
                if (c)
                    c->send_text(msg);
            }
        }

        return crow::response(200);
    });
    

    CROW_ROUTE(app, "/dicePlayed").methods("POST"_method)
    ([](const crow::request& req){
        std::string sessionId = getSessionId(req);

        {
            std::lock_guard<std::mutex> lock(sessionsMutex);
            if (sessions.find(sessionId) == sessions.end())
                return crow::response(403);
        }

        int playerId;
        {
            std::lock_guard<std::mutex> lock(sessionsMutex);
            auto it = sessions.find(sessionId);
            if (it == sessions.end())
                return crow::response(403);

            playerId = it->second;
        }

        engine::Player* player = game.findPlayerById(playerId);

        auto body = crow::json::load(req.body);
        if (!body)
            return crow::response(400);

        int diceIndex = body["dice"].i() - 1;
        auto dice = game.getDices().getDices()[diceIndex];

        std::cout << "Dice played: " << dice.gameColorToString(dice.getColor()) << " and value " << dice.getValue() << std::endl;

        // Update board UI with possible playeble squares with this dice
        crow::json::wvalue message;
        message["type"] = "dicePlayed";
        switch(dice.getColor())
        {
            case engine::GameColor::GameColor::YELLOW:
                message["state"]["yellow"] = player->getPlayebleBoardFieldsAsJSON(player->getYellowBoard(), dice.getValue());
            break;

            case engine::GameColor::GameColor::BLUE:
                message["state"]["blue"] = player->getPlayebleBoardFieldsAsJSON(player->getBlueBoard(), dice.getValue() + game.getDices().getDice(engine::GameColor::GameColor::WHITE).getValue());
            break;

            case engine::GameColor::GameColor::GREEN:
                message["state"]["green"] = player->getPlayebleBoardFieldsAsJSON(player->getGreenBoard(), dice.getValue());
            break;

            case engine::GameColor::GameColor::ORANGE:
                message["state"]["orange"] = player->getPlayebleBoardFieldsAsJSON(player->getOrangeBoard(), dice.getValue());
            break;

            case engine::GameColor::GameColor::PURPLE:
                message["state"]["purple"] = player->getPlayebleBoardFieldsAsJSON(player->getPurpleBoard(), dice.getValue());
            break;

            case engine::GameColor::GameColor::WHITE:
                message["state"]["yellow"] = player->getPlayebleBoardFieldsAsJSON(player->getYellowBoard(), dice.getValue());
                message["state"]["blue"] = player->getPlayebleBoardFieldsAsJSON(player->getBlueBoard(), dice.getValue() + game.getDices().getDice(engine::GameColor::GameColor::BLUE).getValue());
                message["state"]["green"] = player->getPlayebleBoardFieldsAsJSON(player->getGreenBoard(), dice.getValue());
                message["state"]["orange"] = player->getPlayebleBoardFieldsAsJSON(player->getOrangeBoard(), dice.getValue());
                message["state"]["purple"] = player->getPlayebleBoardFieldsAsJSON(player->getPurpleBoard(), dice.getValue());
            break;

            default:
            break;
        }
        
        std::string msg = message.dump();

        {
            std::lock_guard<std::mutex> lock(connectionsMutex);
            for (auto* c : connections) {
                if (c)
                    c->send_text(msg);
            }
        }

        return crow::response(200);
    });

    app.port(18080).multithreaded().run();
}