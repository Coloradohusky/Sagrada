#include "PlayerBoard.hpp"

PlayerBoard::PlayerBoard() {
    for (int i = 0; i < 4; i++) {
        dice.insert(dice.end(), std::vector<Die>());
        for (int j = 0; j < 5; j++) {
            dice[i].insert(dice[i].end(), Die());
        }
    }
}

void PlayerBoard::setFrame(std::vector<std::vector<Die>> windowFrame) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            dice[i][j] = windowFrame[i][j];
        }
    }
}

std::vector<std::vector<Die>> PlayerBoard::getDice() {
    return dice;
}

bool PlayerBoard::isCompletelyEmpty() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            if (!dice[i][j].isCompletelyEmpty()) {
                return false;
            }
        }
    }
    return true;
}

bool PlayerBoard::isEmpty() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            if (!dice[i][j].isEmpty()) {
                return false;
            }
        }
    }
    return true;
}

void PlayerBoard::setDie(std::string data, int index, int subIndex) {
    dice[index][subIndex].set(data);
}

int PlayerBoard::countEmpty() {
    int empty = 0;
    for (int i = 0; i < dice.size(); ++i) {
        for (int j = 0; j < dice[i].size(); ++j) {
            if (dice[i][j].isEmpty()) {
                empty++;
            }
        }
    }
    return empty;
}

int PlayerBoard::countValue(int value) {
    int count = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            if (dice[i][j].getNumber() == value && !dice[i][j].isEmpty()) {
                count++;
            }
        }
    }
    return count;
}

int PlayerBoard::countColor(std::string color) { // returns number of die with that color
    int count = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            if (dice[i][j].getColor() == color && !dice[i][j].isEmpty()) {
                count++;
            }
        }
    }
    return count;
}

int PlayerBoard::countColorTotal(std::string color) { // returns sum of die with that color
    int count = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            if (dice[i][j].getColor() == color && !dice[i][j].isEmpty()) {
                count += dice[i][j].getNumber();
            }
        }
    }
    return count;
}
