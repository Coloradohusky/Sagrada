#include "PatternCard.hpp"

PatternCard::PatternCard() {
	name = "";
	tokens = 0;
	PlayerBoard();
}

void PatternCard::setName(std::string newName) {
	name = newName;
}

void PatternCard::setTokens(int newTokens) {
	tokens = newTokens;
}

std::string PatternCard::getName() {
	return name;
}

int PatternCard::getTokens() {
	return tokens;
}

void PatternCard::setIndex(std::string data, int index, int subIndex) {
	setDie(data, index, subIndex);
}
