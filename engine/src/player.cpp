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
      
    message["yellowBoard"] = std::move(yellowBoardAsString);
    
    return message ;
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