#pragma once
#include <vector>
#include "Die.hpp"

class DiceBag
{
public:
	DiceBag();
	void randomize();
	// polymorphism!
	std::vector<Die> draw(int amount);
	Die draw();
private:
	std::vector<Die> diceBag;
};

