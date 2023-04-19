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
    void setPoints(int points);
    void addPoints(int points);
    bool boardIsEmpty();
    int getTokens();
    std::string getPrivateObjective();
    PlayerBoard getBoard();
    int getPoints();
private:
    int id;
    int favorTokens;
    std::string privateObjective;
    PlayerBoard board;
    int totalPoints;
};

