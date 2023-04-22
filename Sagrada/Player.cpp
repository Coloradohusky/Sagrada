#include "Player.hpp"
#include "Custom.hpp"
#include <string>
#include <set>

Player::Player(int pID) {
    id = pID;
    favorTokens = 0;
    privateObjective = "";
    board = PlayerBoard();
    totalPoints = 0;
}

void Player::setFrame(std::vector<std::vector<Die>> windowFrame) {
    board.setFrame(windowFrame);
}

void Player::setPrivateObjective(std::string privObj) {
    privateObjective = privObj;
}

void Player::setTokens(int newTokens) {
    favorTokens = newTokens;
}

void Player::setDieInBoard(int x, int y, std::string newDie) {
    board.setDie(newDie, x, y);
}

void Player::setDieInBoard(int x, int y, Die newDie) {
    board.setDie(newDie.getColor() + " " + std::to_string(newDie.getNumber()), x, y);
}

void Player::setTotalPoints(int points) {
    totalPoints = points;
}

void Player::addPublicObjectivePoints(int points) {
    publicObjectivePoints.push_back(points);
}

void Player::setPrivateObjectivePoints(int points) {
    privateObjectivePoints = points;
}

bool Player::boardIsEmpty() {
    return board.isCompletelyEmpty();
}

int Player::getTokens() {
    return favorTokens;
}

sf::Color Player::getPrivateObjective() {
	if (privateObjective == "Blue") {
		return blue;
	}
	else if (privateObjective == "Red") {
		return red;
	}
	else if (privateObjective == "Green") {
		return green;
	}
	else if (privateObjective == "Purple") {
		return purple;
	}
	else if (privateObjective == "Yellow") {
		return yellow;
	}
	else {
		return sf::Color::Black;
	}
}

PlayerBoard Player::getBoard() {
    return board;
}

int Player::getTotalPoints() {
	return totalPoints;
}

int Player::getPrivateObjectivePoints() {
	return privateObjectivePoints;
}

int Player::getPublicObjectivePoints(int index) {
	if (index > -1 && index < publicObjectivePoints.size()) {
		return publicObjectivePoints.at(index);
	}
}

int Player::getTotalPoints(std::vector<PublicObjective> selectedPublicObjectives) {
	int pointTotal = 0;
	publicObjectivePoints.clear();
	for (int j = 0; j < selectedPublicObjectives.size(); j++) {
		if (selectedPublicObjectives.at(j).getName() == "Light Shades") {
			// Sets of 1 & 2 values anywhere (2 pts each)
			int ones = 0; int twos = 0;
			ones = board.countValue(1);
			twos = board.countValue(2);
			pointTotal += std::min(ones, twos) * std::stoi(selectedPublicObjectives.at(j).getPoints());
		}
		else if (selectedPublicObjectives.at(j).getName() == "Medium Shades") {
			// Sets of 3 & 4 values anywhere (2 pts each)
			int threes = 0; int fours = 0;
			threes = board.countValue(3);
			fours = board.countValue(4);
			pointTotal += std::min(threes, fours) * std::stoi(selectedPublicObjectives.at(j).getPoints());
		}
		else if (selectedPublicObjectives.at(j).getName() == "Deep Shades") {
			// Sets of 5 & 6 values anywhere (2 pts each)
			int fives = 0; int sixes = 0;
			fives = board.countValue(5);
			sixes = board.countValue(6);
			pointTotal += std::min(fives, sixes) * std::stoi(selectedPublicObjectives.at(j).getPoints());
		}
		else if (selectedPublicObjectives.at(j).getName() == "Column Shade Variety") {
			// Columns with no repeated values (4 pts each)
			int count = 0; // counter variable for columns with no repeats
			for (int col = 0; col < 5; col++) {
				std::set<int> numbers;
				bool hasRepeat = false;
				for (int row = 0; row < 4; row++) {
					int num = board.getDice()[row][col].getNumber();
					if (numbers.count(num) > 0 || board.getDice()[row][col].isEmpty()) {
						hasRepeat = true;
						break;
					}
					numbers.insert(num);
				}
				if (!hasRepeat) {
					count++;
				}
			}
			pointTotal += count * std::stoi(selectedPublicObjectives.at(j).getPoints());
		}
		else if (selectedPublicObjectives.at(j).getName() == "Column Color Variety") {
			// Columns with no repeated colors (5 pts each)
			int count = 0; // counter variable for columns with no repeats
			for (int col = 0; col < 5; col++) {
				std::set<std::string> colors;
				bool hasRepeat = false;
				for (int row = 0; row < 4; row++) {
					std::string color = board.getDice()[row][col].getColor();
					if (colors.count(color) > 0 || board.getDice()[row][col].isEmpty()) {
						hasRepeat = true;
						break;
					}
					colors.insert(color);
				}
				if (!hasRepeat) {
					count++;
				}
			}
			pointTotal += count * std::stoi(selectedPublicObjectives.at(j).getPoints());
		}
		else if (selectedPublicObjectives.at(j).getName() == "Row Shade Variety") {
			// Rows with no repeated values (5 pts each)
			int count = 0; // counter variable for columns with no repeats
			for (int row = 0; row < 4; row++) {
				std::set<int> numbers;
				bool hasRepeat = false;
				for (int col = 0; col < 5; col++) {
					int num = board.getDice()[row][col].getNumber();
					if (numbers.count(num) > 0 || board.getDice()[row][col].isEmpty()) {
						hasRepeat = true;
						break;
					}
					numbers.insert(num);
				}
				if (!hasRepeat) {
					count++;
				}
			}
			pointTotal += count * std::stoi(selectedPublicObjectives.at(j).getPoints());
		}
		else if (selectedPublicObjectives.at(j).getName() == "Row Color Variety") {
			// Rows with no repeated colors (6 pts each)
			int count = 0; // counter variable for columns with no repeats
			for (int row = 0; row < 4; row++) {
				std::set<std::string> colors;
				bool hasRepeat = false;
				for (int col = 0; col < 5; col++) {
					std::string color = board.getDice()[row][col].getColor();
					if (colors.count(color) > 0 || board.getDice()[row][col].isEmpty()) {
						hasRepeat = true;
						break;
					}
					colors.insert(color);
				}
				if (!hasRepeat) {
					count++;
				}
			}
			pointTotal += count * std::stoi(selectedPublicObjectives.at(j).getPoints());
		}
		else if (selectedPublicObjectives.at(j).getName() == "Shade Variety") {
			// Sets of one of each value anywhere (5 each)
			std::vector<int> counts;
			for (int i = 1; i <= 6; i++) {
				counts.push_back(board.countValue(i));
			}
			pointTotal += *std::max_element(counts.begin(), counts.end()) * std::stoi(selectedPublicObjectives.at(j).getPoints());
		}
		else if (selectedPublicObjectives.at(j).getName() == "Color Variety") {
			// Sets of one of each color anywhere (4 each)
			std::vector<int> counts;
			counts.push_back(board.countColor("Blue"));
			counts.push_back(board.countColor("Red"));
			counts.push_back(board.countColor("Green"));
			counts.push_back(board.countColor("Purple"));
			counts.push_back(board.countColor("Yellow"));
			pointTotal += *std::min_element(counts.begin(), counts.end()) * std::stoi(selectedPublicObjectives.at(j).getPoints());
		}
		else if (selectedPublicObjectives.at(j).getName() == "Color Diagonals") {
			// Count of diagonally adjacent same-color dice
			int count = 0;
			std::vector<bool> alreadyCounted(20, false);
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 5; ++j) {
					std::string color = board.getDice()[i][j].getColor();
					if (board.getDice()[i][j].isEmpty()) {
						color = "NONE";
					}
					// Check diagonal neighbors
					if (i > 0 && j > 0 && board.getDice()[i - 1][j - 1].getColor() == color) {
						int index = (i - 1) * 5 + (j - 1);
						if (!alreadyCounted[index]) {
							count++;
							alreadyCounted[index] = true;
						}
					}
					if (i > 0 && j < 5 - 1 && board.getDice()[i - 1][j + 1].getColor() == color) {
						int index = (i - 1) * 5 + (j + 1);
						if (!alreadyCounted[index]) {
							count++;
							alreadyCounted[index] = true;
						}
					}
					if (i < 4 - 1 && j > 0 && board.getDice()[i + 1][j - 1].getColor() == color) {
						int index = (i + 1) * 5 + (j - 1);
						if (!alreadyCounted[index]) {
							count++;
							alreadyCounted[index] = true;
						}
					}
					if (i < 4 - 1 && j < 5 - 1 && board.getDice()[i + 1][j + 1].getColor() == color) {
						int index = (i + 1) * 5 + (j + 1);
						if (!alreadyCounted[index]) {
							count++;
							alreadyCounted[index] = true;
						}
					}
				}
			}
			pointTotal += count;
		}
		int toPushBack = pointTotal;
		for (int k = 0; k < j; k++) {
			toPushBack -= publicObjectivePoints.at(k);
		}
		publicObjectivePoints.push_back(toPushBack);
	}
	// add up private objective
	privateObjectivePoints = board.countColorTotal(privateObjective);
	pointTotal += privateObjectivePoints;
	// lose one point for each empty space
	pointTotal -= board.countEmpty();
	// add one point for each remaining favor token
	// (removed due to no tool card functionality)
	// players.at(i).addPublicObjectivePoints(players.at(i).getTokens());
	totalPoints = pointTotal;
	return totalPoints;
}