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
    bool isCompletelyEmpty();
    void setDie(std::string data, int index, int subIndex);
    bool isEmpty();
    int countEmpty();
    int countValue(int value);
    int countColor(std::string color); // counts the amount of that color
    int countColorSum(std::string color); // adds up the sum of the dice of that color (for private objective)
private:
    std::vector<std::vector<Die>> dice;
};

