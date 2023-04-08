#include "DicePool.hpp"
#include "Custom.hpp"

bool DicePool::isEmpty() {
	return draftPool.empty();
}

void DicePool::roll(DiceBag* diceBag, int playerCount) {
	draftPool = diceBag->draw(playerCount * 2 + 1);
}

int DicePool::size() {
	return draftPool.size();
}

sf::Color DicePool::getDieColor(int index)
{
	if (draftPool[index].getColor() == "Blue") {
		return blue;
	}
	else if (draftPool[index].getColor() == "Red") {
		return red;
	}
	else if (draftPool[index].getColor() == "Green") {
		return green;
	}
	else if (draftPool[index].getColor() == "Purple") {
		return purple;
	}
	else if (draftPool[index].getColor() == "Yellow") {
		return yellow;
	}
	else {
		return sf::Color::Black;
	}
}

Die DicePool::getDie(int index) {
	return draftPool[index];
}
