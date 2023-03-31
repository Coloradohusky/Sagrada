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

std::string PlayerBoard::diceToString() {
    return dice[0][4].toString(); // TODO
}

void PlayerBoard::setDie(std::string data, int index, int subIndex) {
    dice[index][subIndex].set(data);
}
