#pragma once
#include "DiceBag.hpp"
#include <vector>

class DicePool
{
public:
	bool isEmpty();
	void roll(DiceBag diceBag, int playerCount);
private:
	std::vector<Die> draftPool;
};

