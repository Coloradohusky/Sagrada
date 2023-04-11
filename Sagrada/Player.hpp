#pragma once
#include <iostream>
#include "PlayerBoard.hpp"

class Player
{
public:
    Player(int pID);
    void setFrame(std::vector<std::vector<Die>> windowFrame);
    void setPrivateObjective(std::string privObj);
    void setTokens(int newTokens);
    void setDieInBoard(int x, int y, std::string newDie);
    void setDieInBoard(int x, int y, Die newDie);
    std::string toString();
    bool boardIsEmpty();
    int getTokens();
    std::string getPrivateObjective();
    PlayerBoard getBoard();
private:
    int id;
    int favorTokens;
    std::string privateObjective;
    PlayerBoard board;
};

