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

YellowBoard Player::getYellowBoard() const {
    return yellowBoard;
}

BlueBoard Player::getBlueBoard() const {
    return blueBoard;
}

GreenBoard Player::getGreenBoard() const {
    return greenBoard;
}

OrangeBoard Player::getOrangeBoard() const {
    return orangeBoard;
}

PurpleBoard Player::getPurpleBoard() const {
    return purpleBoard;
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

crow::json::wvalue Player::getPlayableFieldsAsJSON(std::unordered_map<GameColor::GameColor, int> dices) {
    crow::json::wvalue message;

    message["yellow"] = getPlayebleBoardFieldsAsJSON(yellowBoard, dices[GameColor::YELLOW], dices[GameColor::WHITE]);
    message["blue"] = getPlayebleBoardFieldsAsJSON(blueBoard, dices[GameColor::BLUE] + dices[GameColor::WHITE]);
    message["green"] = getPlayebleBoardFieldsAsJSON(greenBoard, dices[GameColor::GREEN], dices[GameColor::WHITE]);
    message["orange"] = getPlayebleBoardFieldsAsJSON(orangeBoard, dices[GameColor::ORANGE], dices[GameColor::WHITE]);
    message["purple"] = getPlayebleBoardFieldsAsJSON(purpleBoard, dices[GameColor::PURPLE], dices[GameColor::WHITE]);

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