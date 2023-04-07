#pragma once
#include "DiceBag.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class DicePool
{
public:
	bool isEmpty();
	void roll(DiceBag* diceBag, int playerCount);
	int size();
	sf::Color getDieColor(int index);
private:
	std::vector<Die> draftPool;
};

