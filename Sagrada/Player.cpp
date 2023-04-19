#include "Player.hpp"
#include <string>
#include <vector>

Player::Player(int pID) {
    id = pID;
    favorTokens = 0;
    privateObjective = "";
    board = PlayerBoard();
    totalPoints = 0;
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

void Player::setDieInBoard(int x, int y, std::string newDie) {
    board.setDie(newDie, x, y);
}

void Player::setDieInBoard(int x, int y, Die newDie) {
    board.setDie(newDie.getColor() + " " + std::to_string(newDie.getNumber()), x, y);
}

void Player::setPoints(int points) {
    totalPoints = points;
}

void Player::addPoints(int points) {
    totalPoints += points;
}

bool Player::boardIsEmpty() {
    return board.isCompletelyEmpty();
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

int Player::getPoints() {
    return totalPoints;
}
