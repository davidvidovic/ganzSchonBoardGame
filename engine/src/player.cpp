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

std::string_view Player::getName() {
    return name;
}

int Player::getId() {
    return id;
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