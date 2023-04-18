#include "PublicObjective.hpp"

PublicObjective::PublicObjective() {
    name = "";
    description = "";
    points = "";
}

void PublicObjective::setName(std::string name) {
    this->name = name;
}

void PublicObjective::setDescription(std::string description) {
    this->description = description;
}

void PublicObjective::setPoints(std::string points) {
    this->points = points;
}

std::string PublicObjective::getName() {
    return name;
}

std::string PublicObjective::getDescription() {
    return description;
}

std::string PublicObjective::getPoints() {
    return points;
}
