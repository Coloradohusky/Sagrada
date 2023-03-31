#pragma once
#include <vector>
#include "Die.hpp"
class DiceBag
{
public:
	DiceBag();
	void randomize();
	std::vector<Die> draw(int amount);
private:
	std::vector<Die> diceBag;
};

