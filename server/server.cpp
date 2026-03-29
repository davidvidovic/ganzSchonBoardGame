#include "crow.h"
#include "../engine/include/gameEngine.h"
#include "../engine/include/gameColor.h"

#include <unordered_map>
#include <random>
#include <mutex>
#include <algorithm>
#include <vector>
#include <string>
#include <optional>
#include <iostream>

engine::GameEngine game;

struct SessionInfo
{
    int playerId{};
    crow::websocket::connection* conn{};
    bool hasName{false};
};

std::unordered_map<std::string, SessionInfo> sessionsById;
std::unordered_map<crow::websocket::connection*, std::string> sessionIdByConnection;
std::mutex stateMutex;

std::string generateSessionId()
{
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, static_cast<int>(sizeof(charset) - 2));

    std::string result;
    result.resize(32);

    for (auto& c : result)
        c = charset[dist(gen)];

    return result;
}

int generatePlayerId()
{
    thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(1, 1000000000);
    return dist(gen);
}

std::string getSessionIdFromCookie(const crow::request& req)
{
    const std::string cookie = req.get_header_value("Cookie");
    const std::string key = "session_id=";

    auto pos = cookie.find(key);
    if (pos == std::string::npos)
        return "";

    pos += key.size();

    auto end = cookie.find(";", pos);
    if (end == std::string::npos)
        end = cookie.size();

    return cookie.substr(pos, end - pos);
}

std::string getSessionIdFromRequest(const crow::request& req, const crow::json::rvalue* body = nullptr)
{
    if (body && body->has("sessionId"))
    {
        try
        {
            return std::string((*body)["sessionId"].s());
        }
        catch (...)
        {
        }
    }

    return getSessionIdFromCookie(req);
}

bool sessionExistsLocked(const std::string& sessionId)
{
    return sessionsById.find(sessionId) != sessionsById.end();
}

engine::Player* findPlayerBySessionLocked(const std::string& sessionId)
{
    auto it = sessionsById.find(sessionId);
    if (it == sessionsById.end())
        return nullptr;

    return game.findPlayerById(it->second.playerId);
}

crow::json::wvalue buildDiceListLocked()
{
    crow::json::wvalue diceList = crow::json::wvalue::list();
    auto dices = game.getDices().getDices();

    for (std::size_t i = 0; i < dices.size(); ++i)
    {
        crow::json::wvalue obj;
        obj["color"] = engine::GameColor::gameColorToString(dices[i].getColor());
        obj["value"] = dices[i].getValue();
        obj["state"] = engine::DiceState::diceStateToString(dices[i].getState());
        diceList[i] = std::move(obj);
    }

    return diceList;
}

crow::json::wvalue buildPlayersMessageLocked()
{
    crow::json::wvalue namesList = crow::json::wvalue::list();
    std::size_t index = 0;
    std::size_t numberOfNamedPlayers = 0;

    for (auto& [sessionId, info] : sessionsById)
    {
        if (!info.hasName)
            continue;

        engine::Player* player = game.findPlayerById(info.playerId);
        if (!player)
            continue;

        crow::json::wvalue obj;
        obj["name"] = std::string(player->getName());
        namesList[index++] = std::move(obj);
        ++numberOfNamedPlayers;
    }

    crow::json::wvalue message;
    message["type"] = "playerJoined";
    message["numberOfPlayers"] = static_cast<int>(numberOfNamedPlayers);
    message["players"] = std::move(namesList);
    message["startGame"] = (numberOfNamedPlayers == 4) ? "true" : "false";

    return message;
}

crow::json::wvalue buildUpdateBoardMessageLocked(engine::Player* player)
{
    crow::json::wvalue message;
    message["type"] = "updateBoard";
    message["state"] = player->getBoardAsJSON();
    message["playerState"] = game.getPlayersTurn();
    message["dice"] = buildDiceListLocked();
    return message;
}

void broadcastTextLocked(const std::string& msg)
{
    for (auto& [sessionId, info] : sessionsById)
    {
        if (info.conn)
            info.conn->send_text(msg);
    }
}

void sendTextToSessionLocked(const std::string& sessionId, const std::string& msg)
{
    auto it = sessionsById.find(sessionId);
    if (it == sessionsById.end())
        return;

    if (it->second.conn)
        it->second.conn->send_text(msg);
}

crow::response badRequest(const std::string& text = "Bad request")
{
    return crow::response(400, text);
}

crow::response forbidden(const std::string& text = "Forbidden")
{
    return crow::response(403, text);
}

crow::response notFound(const std::string& text = "Not found")
{
    return crow::response(404, text);
}

int main()
{
    crow::SimpleApp app;
    crow::mustache::set_global_base("server/templates");

    CROW_WEBSOCKET_ROUTE(app, "/ws")
    .onopen([&](crow::websocket::connection& conn) {
        std::lock_guard<std::mutex> lock(stateMutex);

        std::string sessionId;
        do
        {
            sessionId = generateSessionId();
        }
        while (sessionsById.find(sessionId) != sessionsById.end());

        int playerId{};
        bool unique = false;
        while (!unique)
        {
            playerId = generatePlayerId();
            unique = true;

            for (const auto& [existingSessionId, info] : sessionsById)
            {
                if (info.playerId == playerId)
                {
                    unique = false;
                    break;
                }
            }
        }

        engine::Player player(playerId);
        game.addPlayer(player);

        sessionsById[sessionId] = SessionInfo{
            .playerId = playerId,
            .conn = &conn,
            .hasName = false
        };

        sessionIdByConnection[&conn] = sessionId;

        crow::json::wvalue message;
        message["type"] = "sessionCreated";
        message["sessionId"] = sessionId;

        conn.send_text(message.dump());

        std::cout << "[ws open] sessionId=" << sessionId
                  << " playerId=" << playerId << std::endl;
    })
    .onclose([&](crow::websocket::connection& conn, const std::string&, uint16_t) {
        std::lock_guard<std::mutex> lock(stateMutex);

        auto sessionIt = sessionIdByConnection.find(&conn);
        if (sessionIt == sessionIdByConnection.end())
            return;

        const std::string sessionId = sessionIt->second;
        auto infoIt = sessionsById.find(sessionId);

        int playerId = -1;
        if (infoIt != sessionsById.end())
            playerId = infoIt->second.playerId;

        sessionIdByConnection.erase(sessionIt);
        if (infoIt != sessionsById.end())
            sessionsById.erase(infoIt);

        std::cout << "[ws close] sessionId=" << sessionId
                  << " playerId=" << playerId << std::endl;

        crow::json::wvalue playersMessage = buildPlayersMessageLocked();
        broadcastTextLocked(playersMessage.dump());

        // game.removePlayerById(playerId);
    })
    .onmessage([&](crow::websocket::connection&, const std::string&, bool) {
        // Not used right now.
    });

    CROW_ROUTE(app, "/")([]() {
        auto page = crow::mustache::load_text("board.html");
        return crow::response(page);
    });

    CROW_ROUTE(app, "/board")([]() {
        auto page = crow::mustache::load_text("board.html");
        return crow::response(page);
    });

    CROW_ROUTE(app, "/board.js")([]() {
        crow::response res;
        res.code = 200;
        res.set_header("Content-Type", "application/javascript; charset=UTF-8");
        res.body = crow::mustache::load_text("board.js");
        return res;
    });

    CROW_ROUTE(app, "/setName").methods("POST"_method)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("name"))
            return badRequest("Missing name");

        const std::string sessionId = getSessionIdFromRequest(req, &body);
        if (sessionId.empty())
            return forbidden("Missing sessionId");

        const std::string playerName = std::string(body["name"].s());

        std::lock_guard<std::mutex> lock(stateMutex);

        auto sessionIt = sessionsById.find(sessionId);
        if (sessionIt == sessionsById.end())
            return forbidden("Invalid sessionId");

        engine::Player* player = game.findPlayerById(sessionIt->second.playerId);
        if (!player)
            return notFound("Player not found");

        player->setName(playerName);
        sessionIt->second.hasName = true;

        int namedPlayers = 0;
        for (const auto& [id, info] : sessionsById)
        {
            if (info.hasName)
                ++namedPlayers;
        }

        if (namedPlayers == 4)
        {
            game.startGame();
        }

        crow::json::wvalue playersMessage = buildPlayersMessageLocked();
        broadcastTextLocked(playersMessage.dump());

        crow::response res(200);
        res.add_header("Set-Cookie", "session_id=" + sessionId + "; Path=/; SameSite=Lax");
        return res;
    });

    CROW_ROUTE(app, "/playerJoined").methods("POST"_method)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        const std::string sessionId = getSessionIdFromRequest(req, body ? &body : nullptr);

        if (sessionId.empty())
            return forbidden("Missing sessionId");

        std::lock_guard<std::mutex> lock(stateMutex);

        if (!sessionExistsLocked(sessionId))
            return forbidden("Invalid sessionId");

        crow::json::wvalue playersMessage = buildPlayersMessageLocked();
        sendTextToSessionLocked(sessionId, playersMessage.dump());

        return crow::response(200);
    });

    CROW_ROUTE(app, "/playerLeft").methods("POST"_method)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        const std::string sessionId = getSessionIdFromRequest(req, body ? &body : nullptr);

        if (sessionId.empty())
            return forbidden("Missing sessionId");

        std::lock_guard<std::mutex> lock(stateMutex);

        auto it = sessionsById.find(sessionId);
        if (it == sessionsById.end())
            return forbidden("Invalid sessionId");

        crow::websocket::connection* conn = it->second.conn;
        if (conn)
        {
            sessionIdByConnection.erase(conn);
        }

        sessionsById.erase(it);

        crow::json::wvalue playersMessage = buildPlayersMessageLocked();
        broadcastTextLocked(playersMessage.dump());

        return crow::response(200);
    });

    CROW_ROUTE(app, "/rollDice").methods("POST"_method)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        const std::string sessionId = getSessionIdFromRequest(req, body ? &body : nullptr);

        if (sessionId.empty())
            return forbidden("Missing sessionId");

        std::lock_guard<std::mutex> lock(stateMutex);

        auto sessionIt = sessionsById.find(sessionId);
        if (sessionIt == sessionsById.end())
            return forbidden("Invalid sessionId");

        engine::Player* player = game.findPlayerById(sessionIt->second.playerId);
        if (!player)
            return notFound("Player not found");

        game.getDices().rollDices();
        game.getDices().sortDices();

        crow::json::wvalue message;
        message["type"] = "diceRolled";
        message["dice"] = buildDiceListLocked();
        message["playebleFields"] = player->getPlayableFieldsAsJSON(game.getDiceValues());

        broadcastTextLocked(message.dump());

        return crow::response(200);
    });

    CROW_ROUTE(app, "/updateBoard").methods("POST"_method)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        const std::string sessionId = getSessionIdFromRequest(req, body ? &body : nullptr);

        if (sessionId.empty())
            return forbidden("Missing sessionId");

        std::lock_guard<std::mutex> lock(stateMutex);

        auto sessionIt = sessionsById.find(sessionId);
        if (sessionIt == sessionsById.end())
            return forbidden("Invalid sessionId");

        engine::Player* player = game.findPlayerById(sessionIt->second.playerId);
        if (!player)
            return notFound("Player not found");

        crow::json::wvalue message = buildUpdateBoardMessageLocked(player);
        broadcastTextLocked(message.dump());

        return crow::response(200);
    });

    CROW_ROUTE(app, "/dicePlayed").methods("POST"_method)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("dice"))
            return badRequest("Missing dice");

        const std::string sessionId = getSessionIdFromRequest(req, &body);
        if (sessionId.empty())
            return forbidden("Missing sessionId");

        std::lock_guard<std::mutex> lock(stateMutex);

        auto sessionIt = sessionsById.find(sessionId);
        if (sessionIt == sessionsById.end())
            return forbidden("Invalid sessionId");

        engine::Player* player = game.findPlayerById(sessionIt->second.playerId);
        if (!player)
            return notFound("Player not found");

        int diceIndex = body["dice"].i() - 1;
        const auto dices = game.getDices().getDices();

        if (diceIndex < 0 || diceIndex >= static_cast<int>(dices.size()))
            return badRequest("Invalid dice index");

        auto dice = dices[diceIndex];
        game.setDiceColorLastPlayed(dice.getColor());
        game.setDiceIndexLastPlayed(diceIndex);

        std::cout << "Dice played by playerId=" << sessionIt->second.playerId
                  << ": " << engine::GameColor::gameColorToString(dice.getColor())
                  << " value=" << dice.getValue() << std::endl;

        crow::json::wvalue message;
        message["type"] = "dicePlayed";

        switch (dice.getColor())
        {
            case engine::GameColor::GameColor::YELLOW:
                message["state"]["yellow"] =
                    player->getPlayebleBoardFieldsAsJSON(player->getYellowBoard(), dice.getValue());
                break;

            case engine::GameColor::GameColor::BLUE:
                message["state"]["blue"] =
                    player->getPlayebleBoardFieldsAsJSON(
                        player->getBlueBoard(),
                        dice.getValue() + game.getDices().getDice(engine::GameColor::GameColor::WHITE).getValue());
                break;

            case engine::GameColor::GameColor::GREEN:
                message["state"]["green"] =
                    player->getPlayebleBoardFieldsAsJSON(player->getGreenBoard(), dice.getValue());
                break;

            case engine::GameColor::GameColor::ORANGE:
                message["state"]["orange"] =
                    player->getPlayebleBoardFieldsAsJSON(player->getOrangeBoard(), dice.getValue());
                break;

            case engine::GameColor::GameColor::PURPLE:
                message["state"]["purple"] =
                    player->getPlayebleBoardFieldsAsJSON(player->getPurpleBoard(), dice.getValue());
                break;

            case engine::GameColor::GameColor::WHITE:
                message["state"]["yellow"] =
                    player->getPlayebleBoardFieldsAsJSON(player->getYellowBoard(), dice.getValue());
                message["state"]["blue"] =
                    player->getPlayebleBoardFieldsAsJSON(
                        player->getBlueBoard(),
                        dice.getValue() + game.getDices().getDice(engine::GameColor::GameColor::BLUE).getValue());
                message["state"]["green"] =
                    player->getPlayebleBoardFieldsAsJSON(player->getGreenBoard(), dice.getValue());
                message["state"]["orange"] =
                    player->getPlayebleBoardFieldsAsJSON(player->getOrangeBoard(), dice.getValue());
                message["state"]["purple"] =
                    player->getPlayebleBoardFieldsAsJSON(player->getPurpleBoard(), dice.getValue());
                break;

            default:
                break;
        }

        sendTextToSessionLocked(sessionId, message.dump());

        return crow::response(200);
    });

    CROW_ROUTE(app, "/playMove").methods("POST"_method)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("board") || !body.has("index"))
            return badRequest("Missing board information");

        const std::string sessionId = getSessionIdFromRequest(req, &body);
        if (sessionId.empty())
            return forbidden("Missing sessionId");

        std::lock_guard<std::mutex> lock(stateMutex);

        auto sessionIt = sessionsById.find(sessionId);
        if (sessionIt == sessionsById.end())
            return forbidden("Invalid sessionId");

        engine::Player* player = game.findPlayerById(sessionIt->second.playerId);
        if (!player)
            return notFound("Player not found");

        engine::GameColor::GameColor boardColor;
        try
        {
            boardColor = engine::GameColor::stringToGameColor(body["board"].s());
        }
        catch (const std::exception&)
        {
            return badRequest("Invalid board");
        }

        int boardIndex = body["index"].i();

        std::cout << "Board move played by playerId=" << sessionIt->second.playerId
                  << ": " << boardColor
                  << " index=" << boardIndex << std::endl;

        game.getDices().getDice(game.getDiceColorLastPlayed()).setState(engine::DiceState::PLAYED);

        for (int i = 0; i < game.getDiceIndexLastPlayed(); i++)
        {
            if (game.getDices().getDices()[i].getState() == engine::DiceState::AVAILABLE 
                && game.getDices().getDices()[i].getValue() < game.getDices().getDice(game.getDiceColorLastPlayed()).getValue()
        )
            {
                game.getDices().getDices()[i].setState(engine::DiceState::LOCKED);
            }
        }

        switch (boardColor)
        {
            case engine::GameColor::GameColor::YELLOW:
                player->getYellowBoard().play(boardIndex);
                break;

            case engine::GameColor::GameColor::BLUE:
                player->getBlueBoard().play(boardIndex);
                break;

            case engine::GameColor::GameColor::GREEN:
                player->getGreenBoard().play(boardIndex);
                break;

            case engine::GameColor::GameColor::ORANGE:
                player->getOrangeBoard().play(
                    game.getDices().getDice(game.getDiceColorLastPlayed()).getValue());
                break;

            case engine::GameColor::GameColor::PURPLE:
                player->getPurpleBoard().play(
                    game.getDices().getDice(game.getDiceColorLastPlayed()).getValue());
                break;

            default:
                return badRequest("Unsupported board");
        }

        crow::json::wvalue message = buildUpdateBoardMessageLocked(player);
        sendTextToSessionLocked(sessionId, message.dump());

        return crow::response(200);
    });

    app.port(18080).multithreaded().run();
}