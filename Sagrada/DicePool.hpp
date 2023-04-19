#pragma once
#include "DiceBag.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class DicePool
{
public:
	bool isCompletelyEmpty();
	void roll(DiceBag* diceBag, int playerCount);
	int size();
	sf::Color getDieColor(int index);
	Die getDie(int index);
	void removeDie(int index);
private:
	std::vector<Die> draftPool;
};

