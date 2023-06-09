#include "DicePool.hpp"
#include "Custom.hpp"

bool DicePool::isCompletelyEmpty() {
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
	if (index < 0 || index >= draftPool.size()) {
		return Die();
	}
	return draftPool[index];
}

void DicePool::removeDie(int index) {
	draftPool.erase(draftPool.begin() + index);
}
