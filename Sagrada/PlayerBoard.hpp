#pragma once
#include <iostream>
#include <array>
#include <vector>
#include "Die.hpp"

class PlayerBoard {
public:
    PlayerBoard();
    void setFrame(std::vector<std::vector<Die>> windowFrame);
    std::vector<std::vector<Die>> getDice();
    bool isEmpty();
    std::string diceToString();
    void setDie(std::string data, int index, int subIndex);
private:
    std::vector<std::vector<Die>> dice;
};

