#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Die
{
public:
	Die();
	Die(std::string newColor);
	Die(int newNumber);
	Die(std::string newColor, int newNumber);
	void roll();
	void setNumber(int newNumber);
	void setColor(std::string newColor);
	void set(std::string data);
	bool equals(Die newDie);
	std::string toString();
	bool isEmpty();
	bool isCompletelyEmpty();
	bool isFull();
	std::string getColor();
	sf::Color getFillColor();
	int getNumber();
private:
	std::string color;
	int number;
};

