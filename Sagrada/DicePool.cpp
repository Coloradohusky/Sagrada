#include "DicePool.hpp"

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
		return sf::Color(45, 187, 200, 255); // TODO: include Custom.hpp for global colors (will that work?)
	}
	else if (draftPool[index].getColor() == "Red") {
		return sf::Color(220, 35, 39, 255);
	}
	else if (draftPool[index].getColor() == "Green") {
		return sf::Color(3, 171, 108, 255);
	}
	else if (draftPool[index].getColor() == "Purple") {
		return sf::Color(165, 65, 152, 255);
	}
	else if (draftPool[index].getColor() == "Yellow") {
		return sf::Color(243, 222, 12, 255);
	}
	else {
		return sf::Color::Black;
	}
}
