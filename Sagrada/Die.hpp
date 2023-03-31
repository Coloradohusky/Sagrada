#pragma once
#include <iostream>
class Die
{
public:
	Die();
	Die(std::string newColor);
	void roll();
	void setNumber(int newNumber);
	void set(std::string data);
	bool compare(std::string name);
	std::string toString();
	bool isEmpty();
	std::string getColor();
	int getNumber();
private:
	std::string color;
	int number;
};

