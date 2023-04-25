#include "DiceBag.hpp"
#include <random>
#include <array>

DiceBag::DiceBag() {
	std::vector<std::string> colors = { "Blue", "Red", "Green", "Purple", "Yellow" };
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 18; j++) {
			diceBag.insert(diceBag.begin(), Die(colors[i]));
		}
	}
	this->randomize();
}

void DiceBag::randomize() {
	std::shuffle(std::begin(diceBag), std::end(diceBag), std::default_random_engine());
}

std::vector<Die> DiceBag::draw(int amount) {
	this->randomize();
	std::vector<Die> newDraw;
	for (int i = 0; i < amount; i++) {
		newDraw.insert(newDraw.begin(), diceBag.back());
		diceBag.pop_back();
	}
	// it doesn't work when it's only in one for loop, so I had to split it up
	for (int i = 0; i < amount; i++) {
		newDraw[i].roll();
	}
	return newDraw;
}

Die DiceBag::draw() {
	std::vector<Die> newDraw;
	newDraw.insert(newDraw.begin(), diceBag.back());
	diceBag.pop_back();
	return newDraw.back();
}
