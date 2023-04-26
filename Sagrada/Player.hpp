#pragma once
#include <iostream>
#include "PlayerBoard.hpp"
#include "PublicObjective.hpp"
#include <vector>

#pragma warning(disable: 4267)

class Player
{
public:
    Player(int pID);
    void setFrame(std::vector<std::vector<Die>> windowFrame);
    void setPrivateObjective(std::string privObj);
    void setTokens(int newTokens);
    void setDieInBoard(int x, int y, std::string newDie);
    void setDieInBoard(int x, int y, Die newDie);
    void setTotalPoints(int points);
    void addPublicObjectivePoints(int points);
    void setPrivateObjectivePoints(int points);
    bool boardIsEmpty();
    int getTokens();
    sf::Color getPrivateObjective();
    PlayerBoard getBoard();
    int getTotalPoints(std::vector<PublicObjective> selectedPublicObjectives);
    int getTotalPoints();
    int getPrivateObjectivePoints();
    int getPublicObjectivePoints(int index);
private:
    int id;
    int favorTokens;
    std::string privateObjective;
    PlayerBoard board;
    int totalPoints;
    int privateObjectivePoints;
    std::vector<int> publicObjectivePoints;
};

