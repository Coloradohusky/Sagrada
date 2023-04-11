#pragma once
#include <iostream>
class Die
{
public:
	Die();
	Die(std::string newColor);
	void roll();
	void setNumber(int newNumber);
	void setColor(std::string newColor);
	void set(std::string data);
	bool equals(Die newDie);
	std::string toString();
	bool isEmpty();
	bool isFull();
	std::string getColor();
	int getNumber();
private:
	std::string color;
	int number;
};

