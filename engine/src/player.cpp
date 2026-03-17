#include "../include/player.h"

namespace engine {

int Player::generateId = 0;

Player::Player() : id(generateId++) {
    name = "";
}

Player::Player(std::string_view name) : id(generateId++), name(name) {
}

void Player::setName(std::string_view name) {
    name = name;
}

std::string_view Player::getName() const {
    return name;
}

int Player::getId() {
    return id;
}

void Player::setIsTurn(bool newTurn) {
    isTurn = newTurn;
}

bool Player::getIsTurn() {
    return isTurn;
}

void Player::setBonuses(GameBonus::GameBonus bonus) {
    bonuses.push_back(bonus);
}

std::vector<GameBonus::GameBonus> Player::getBonuses() {
    return bonuses;
}

crow::json::wvalue Player::getBoardAsJSON() {
    crow::json::wvalue message;

    std::string yellowBoardAsString = yellowBoard.getBoardAsString();
    std::string blueBoardAsString = blueBoard.getBoardAsString();
    std::string greenBoardAsString = greenBoard.getBoardAsString();
    std::string orangeBoardAsString = orangeBoard.getBoardAsString();
    std::string purpleBoardAsString = purpleBoard.getBoardAsString();

    message["yellowBoard"] = std::move(yellowBoardAsString);
    message["blueBoard"] = std::move(blueBoardAsString);
    message["greenBoard"] = std::move(greenBoardAsString);
    message["orangeBoard"] = std::move(orangeBoardAsString);
    message["purpleBoard"] = std::move(purpleBoardAsString);
    
    return message ;
}

crow::json::wvalue Player::getPlayebleFieldsAsJSON(std::unordered_map<GameColor::GameColor, int> dices) {
    crow::json::wvalue message;

    int diceValue;
    std::vector<int> playableFields;
    std::vector<int> playableFieldsWhite;

    diceValue = dices[GameColor::YELLOW];
    playableFields = yellowBoard.isPlayable(diceValue);
    diceValue = dices[GameColor::WHITE];
    playableFieldsWhite = yellowBoard.isPlayable(diceValue);
    playableFields.insert(playableFields.end(), playableFieldsWhite.begin(), playableFieldsWhite.end());

    // Yellow
    crow::json::wvalue yellowList = crow::json::wvalue::list();

    for (size_t i = 0; i < playableFields.size(); ++i) {
        yellowList[i] = playableFields[i];
    }

    message["yellow"] = std::move(yellowList);

    // Blue
    diceValue = dices[GameColor::BLUE];
    int whiteDiceValue = dices[GameColor::WHITE];
    playableFields = blueBoard.isPlayable(diceValue + whiteDiceValue);

    crow::json::wvalue blueList = crow::json::wvalue::list();

    for (size_t i = 0; i < playableFields.size(); ++i) {
        blueList[i] = playableFields[i];
    }

    message["blue"] = std::move(blueList);

    // Green
    diceValue = dices[GameColor::GREEN];
    playableFields = greenBoard.isPlayable(diceValue);
    diceValue = dices[GameColor::WHITE];
    playableFieldsWhite = greenBoard.isPlayable(diceValue);
    playableFields.insert(playableFields.end(), playableFieldsWhite.begin(), playableFieldsWhite.end());

    crow::json::wvalue greenList = crow::json::wvalue::list();

    for (size_t i = 0; i < playableFields.size(); ++i) {
        greenList[i] = playableFields[i];
    }

    message["green"] = std::move(greenList);

    // Orange
    diceValue = dices[GameColor::ORANGE];
    playableFields = orangeBoard.isPlayable(diceValue);
    diceValue = dices[GameColor::WHITE];
    playableFieldsWhite = orangeBoard.isPlayable(diceValue);
    playableFields.insert(playableFields.end(), playableFieldsWhite.begin(), playableFieldsWhite.end());

    crow::json::wvalue orangeList = crow::json::wvalue::list();

    for (size_t i = 0; i < playableFields.size(); ++i) {
        orangeList[i] = playableFields[i];
    }

    message["orange"] = std::move(orangeList);

    // Purple
    diceValue = dices[GameColor::PURPLE];
    playableFields = purpleBoard.isPlayable(diceValue);
    diceValue = dices[GameColor::WHITE];
    playableFieldsWhite = purpleBoard.isPlayable(diceValue);
    playableFields.insert(playableFields.end(), playableFieldsWhite.begin(), playableFieldsWhite.end());

    crow::json::wvalue purpleList = crow::json::wvalue::list();

    for (size_t i = 0; i < playableFields.size(); ++i) {
        purpleList[i] = playableFields[i];
    }

    message["purple"] = std::move(purpleList);

    return message;
}

bool operator== (const Player& p1, const Player& p2)
{
    return p1.id == p2.id;
}

std::ostream& operator<<(std::ostream& os, const Player& p) {
        os << p.id << ", " << p.name;
        return os;
    }

}