#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "greenBoard.h"
#include "yellowBoard.h"
#include "blueBoard.h"
#include "orangeBoard.h"
#include "purpleBoard.h"
#include "includes.h"
#include "gameColor.h"
#include "crow.h"

namespace engine {

class Player {
    private:
    int id;
    std::string name;
    bool isTurn{false};
    int score{0};
    GreenBoard greenBoard;
    YellowBoard yellowBoard;
    BlueBoard blueBoard;
    OrangeBoard orangeBoard;
    PurpleBoard purpleBoard;
    std::vector<GameBonus::GameBonus> bonuses;

    public:
    Player(int id);
    Player(int id, std::string_view name);
    void setName(std::string_view playerName);
    std::string_view getName() const;
    int getId();
    void setIsTurn(bool newTurn);
    bool getIsTurn();
    void setBonuses(GameBonus::GameBonus bonus);
    std::vector<GameBonus::GameBonus> getBonuses();

    YellowBoard& getYellowBoard();
    BlueBoard& getBlueBoard();
    GreenBoard& getGreenBoard();
    OrangeBoard& getOrangeBoard();
    PurpleBoard& getPurpleBoard();

    crow::json::wvalue getBoardAsJSON();
    crow::json::wvalue getPlayableFieldsAsJSON(std::unordered_map<GameColor::GameColor, int> dices);

    template <class T>
    crow::json::wvalue getPlayebleBoardFieldsAsJSON(const T& board, int diceValue, int diceValueOptional = 0) {
        crow::json::wvalue messageList = crow::json::wvalue::list();
        std::vector<int> playableFields = board.isPlayable(diceValue);

        if (diceValueOptional != 0) {
            std::vector<int> playableFieldsOptional = board.isPlayable(diceValueOptional);
            playableFields.insert(playableFields.end(),
                                playableFieldsOptional.begin(),
                                playableFieldsOptional.end());
        }

        std::sort(playableFields.begin(), playableFields.end());
        playableFields.erase(std::unique(playableFields.begin(), playableFields.end()),
                            playableFields.end());

        for (size_t i = 0; i < playableFields.size(); ++i) {
            messageList[i] = playableFields[i];
        }

        return messageList;
    }

    friend bool operator== (const Player& p1, const Player& p2);
    friend std::ostream& operator<<(std::ostream& os, const Player& p);
};
}

#endif