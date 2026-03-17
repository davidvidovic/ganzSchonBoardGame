#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include "greenBoard.h"
#include "yellowBoard.h"
#include "blueBoard.h"
#include "orangeBoard.h"
#include "purpleBoard.h"
#include "includes.h"
#include "crow.h"

namespace engine {

class Player {
    private:
    int id;
    std::string name;
    static int generateId;

    bool isTurn{false};
    int score{0};
    GreenBoard greenBoard;
    YellowBoard yellowBoard;
    BlueBoard blueBoard;
    OrangeBoard orangeBoard;
    PurpleBoard purpleBoard;
    std::vector<GameBonus::GameBonus> bonuses;

    public:
    Player();
    Player(std::string_view name);
    void setName(std::string_view name);
    std::string_view getName() const;
    int getId();
    void setIsTurn(bool newTurn);
    bool getIsTurn();
    void setBonuses(GameBonus::GameBonus bonus);
    std::vector<GameBonus::GameBonus> getBonuses();
    crow::json::wvalue getBoardAsJSON();
    crow::json::wvalue getPlayebleFieldsAsJSON(std::unordered_map<GameColor::GameColor, int> dices);

    friend bool operator== (const Player& p1, const Player& p2);
    friend std::ostream& operator<<(std::ostream& os, const Player& p);
};
}

#endif