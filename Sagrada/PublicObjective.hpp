#pragma once
#include <iostream>
class PublicObjective {
public:
    PublicObjective();
    void setName(std::string name);
    void setDescription(std::string description);
    void setPoints(std::string points);
    std::string getName();
    std::string getDescription();
    std::string getPoints();
private:
    std::string name;
    std::string description;
    std::string points; // can be "#"
};

