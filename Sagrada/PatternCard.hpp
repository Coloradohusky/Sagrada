#pragma once
#include <iostream>
#include <array>
#include <vector>
#include "Die.hpp"
#include "PlayerBoard.hpp"

// inheritance!
class PatternCard : public PlayerBoard {
public:
    PatternCard();
    void setName(std::string newName);
    void setTokens(int newTokens);
    std::string getName();
    int getTokens();
    void setIndex(std::string data, int index, int subIndex);
private:
    std::string name;
    int tokens;
};

