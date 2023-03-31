#include "Player.hpp"
#include <string>
#include <vector>

Player::Player(int pID) {
    id = pID;
    favorTokens = 0;
    privateObjective = "";
    board = PlayerBoard();
}

void Player::setFrame(std::vector<std::vector<Die>> windowFrame) {
    board.setFrame(windowFrame);
}

void Player::setPrivateObjective(std::string privObj) {
    privateObjective = privObj;
}

void Player::setTokens(int newTokens) {
    favorTokens = newTokens;
}

std::string Player::toString() {
    return std::to_string(id) + ": " + privateObjective + " " + board.diceToString() + " " + std::to_string(favorTokens);
}

bool Player::boardIsEmpty() {
    return board.isEmpty();
}

int Player::getTokens() {
    return favorTokens;
}

std::string Player::getPrivateObjective() {
    return privateObjective;
}

PlayerBoard Player::getBoard() {
    return board;
}