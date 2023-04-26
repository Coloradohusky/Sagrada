#include "Die.hpp"
#include "Custom.hpp"
#include <string>
#include <random>

Die::Die() {
	color = "";
	number = 0;
}

Die::Die(std::string newColor) {
	color = newColor;
	number = 0;
}

Die::Die(int newNumber) {
	color = "";
	number = newNumber;
}

Die::Die(std::string newColor, int newNumber) {
	color = newColor;
	number = newNumber;
}

void Die::roll() {
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_int_distribution<> dist(1, 6);
	number = dist(e2);
}

void Die::setNumber(int newNumber) {
	number = newNumber;
}

void Die::setColor(std::string newColor) {
	color = newColor;
}

void Die::set(std::string data) {
	if (data.find(" ") != std::string::npos) {
		color = data.substr(0, data.find(" "));
		data.erase(0, data.find(" ") + 1);
		number = std::stoi(data);
	}
	else {
		if (data.length() == 1) {
			number = std::stoi(data);
		}
		else {
			color = data;
		}
	}
}

bool Die::equals(Die newDie) {
	if (color == newDie.getColor() && number == newDie.getNumber()) {
		return true;
	}
	return false;
}

std::string Die::toString() {
	if (number == NULL) {
		return color;
	}
	else {
		return color + " " + std::to_string(number);
	}
	return std::string();
}


bool Die::isCompletelyEmpty() {
	if (color == "" && number == 0) {
		return true;
	}
	return false;
}

bool Die::isEmpty() {
	if (color == "" || number == 0) {
		return true;
	}
	return false;
}

bool Die::isFull() {
	if (color != "" && number != 0) {
		return true;
	}
	return false;
}

std::string Die::getColor() {
	return color;
}

sf::Color Die::getFillColor() {
	if (color == "Blue") {
		return blue;
	}
	else if (color == "Red") {
		return red;
	}
	else if (color == "Green") {
		return green;
	}
	else if (color == "Purple") {
		return purple;
	}
	else if (color == "Yellow") {
		return yellow;
	}
	return lightGray;
}

int Die::getNumber() {
	return number;
}
