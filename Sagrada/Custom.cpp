#include "Custom.hpp"

int SCREEN_WIDTH = 1120;
int SCREEN_HEIGHT = SCREEN_WIDTH * ratio;

sf::ConvexShape RoundedRectangle(float x, float y, float rectWidth, float rectHeight, float radius, const sf::Color& shapeColor, float outline, const sf::Color& outlineCol) {
	int totalPoints = 40;
	sf::ConvexShape rrect;
	rrect.setOutlineThickness(outline);
	rrect.setFillColor(shapeColor);
	rrect.setOutlineColor(outlineCol);
	float X = 0, Y = 0;
	int total = 0;
	rrect.setPointCount(totalPoints);
	for (int i = 0; i < (totalPoints / 4); i++) {
		X += radius / (totalPoints / 4);
		Y = sqrt(radius * radius - X * X);
		rrect.setPoint(total, sf::Vector2f(X + x + rectWidth - radius, y - Y + radius));
		total++;
	}
	Y = 0;
	for (int i = 0; i < (totalPoints / 4); i++) {
		Y += radius / (totalPoints / 4);
		X = sqrt(radius * radius - Y * Y);
		rrect.setPoint(total, sf::Vector2f(x + rectWidth + X - radius, y + rectHeight - radius + Y));
		total++;
	}
	X = 0;
	for (int i = 0; i < (totalPoints / 4); i++) {
		X += radius / (totalPoints / 4);
		Y = sqrt(radius * radius - X * X);
		rrect.setPoint(total, sf::Vector2f(x + radius - X, y + rectHeight - radius + Y));
		total++;
	}
	Y = 0;
	for (int i = 0; i < (totalPoints / 4); i++) {
		Y += radius / (totalPoints / 4);
		X = sqrt(radius * radius - Y * Y);
		rrect.setPoint(total, sf::Vector2f(x - X + radius, y + radius - Y));
		total++;
	}
	return rrect;
}

bool MouseInConvexShape(sf::ConvexShape shape, sf::Window* window) {
	sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(*window));
	if (shape.getGlobalBounds().contains(mousePos)) {
		return true;
	}
	return false;
}

// placement_restrictions are the placement restrictions that should be ignored
// Normal = Obey all
// Value = Ignore value restrictions (Copper Foil Burnisher)
// Color = Ignore color restrictions (Eglomise Brush)
// Adjacent = Ignore adjacency restrictions (Cork-backed Straightedge)
std::vector<std::vector<bool>> determinePlacementSpots(std::vector<std::vector<Die>> board, Die selectedDie, std::string restrictions) {
	std::vector<std::vector<bool>> placementSpots = {
		{true, true, true, true, true},
		{true, true, true, true, true},
		{true, true, true, true, true},
		{true, true, true, true, true}
	};
	if (selectedDie.getNumber() == 0) {
		return {
		{false, false, false, false, false},
		{false, false, false, false, false},
		{false, false, false, false, false},
		{false, false, false, false, false}
		};
	}
	// do checks
	for (int i = 0; i < board.size(); i++) {
		for (int j = 0; j < board[0].size(); j++) {
			// TODO: dice of the same number/color CANNOT be placed next to each other
			// check for already-placed dice
			if (true) { // makes for easier collapsing of sections
				if (board[i][j].isFull()) {
					placementSpots[i][j] = false;
				}
			}
			// check for values
			if (restrictions != "Value") {
				if (board[i][j].getNumber() != selectedDie.getNumber() && board[i][j].getNumber() != 0) {
					placementSpots[i][j] = false;
				}
				if (board[i][j].getNumber() == selectedDie.getNumber()) {
					if (i + 1 < 4) {
						placementSpots[i + 1][j] = false;
					}
					if (i - 1 > -1) {
						placementSpots[i - 1][j] = false;
					}
					if (j + 1 < 5) {
						placementSpots[i][j + 1] = false;
					}
					if (j - 1 > -1) {
						placementSpots[i][j - 1] = false;
					}
				}
			}
			// check for colors
			if (restrictions != "Color") {
				if (board[i][j].getColor() != selectedDie.getColor() && board[i][j].getColor() != "") {
					placementSpots[i][j] = false;
				}
				if (board[i][j].getColor() == selectedDie.getColor()) {
					if (i + 1 < 4) {
						placementSpots[i + 1][j] = false;
					}
					if (i - 1 > -1) {
						placementSpots[i - 1][j] = false;
					}
					if (j + 1 < 5) {
						placementSpots[i][j + 1] = false;
					}
					if (j - 1 > -1) {
						placementSpots[i][j - 1] = false;
					}
				}
			}
			// check for adjacency
			if (restrictions != "Adjacent") {
				bool there_are_dice = false;
				for (std::vector<Die> x : board) { // First, check if there are ANY dice in the board.
					for (Die y : x) {
						if (y.isFull()) {
							there_are_dice = true;
						}
					}
				}
				if (!there_are_dice) { // If there aren't any dice in the window, then dice can ONLY be placed on the outside spots
					placementSpots[1][1] = false;
					placementSpots[1][2] = false;
					placementSpots[1][3] = false;
					placementSpots[2][1] = false;
					placementSpots[2][2] = false;
					placementSpots[2][3] = false;
				}
				else { // Otherwise, they can only be placed next to existing dice
					std::array<std::array<bool, 5>, 4> toCheck {{
								{{false, false, false, false, false}},
								{{false, false, false, false, false}},
								{{false, false, false, false, false}},
								{{false, false, false, false, false}}
						}};
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 5; j++) {
							if (board[i][j].isFull()) {
								if (i + 1 < 4) {
									toCheck[i + 1][j] = true;
								}
								if (i - 1 > -1) {
									toCheck[i - 1][j] = true;
								}
								if (j + 1 < 5) {
									toCheck[i][j + 1] = true;
								}
								if (j - 1 > -1) {
									toCheck[i][j - 1] = true;
								}
								if (i + 1 < 4 && j + 1 < 5) {
									toCheck[i + 1][j + 1] = true;
								}
								if (i + 1 < 4 && j - 1 > -1) {
									toCheck[i + 1][j - 1] = true;
								}
								if (i - 1 > -1 && j + 1 < 5) {
									toCheck[i - 1][j + 1] = true;
								}
								if (i - 1 > -1 && j - 1 > -1) {
									toCheck[i - 1][j - 1] = true;
								}
							}
						}
					}
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 5; j++) {
							if (toCheck[i][j] == false) {
								placementSpots[i][j] = false;
							}
						}
					}
				}
			}
		}
	}
	return placementSpots;
}

// Fills pattern cards with info from JSON
void fillFrameArray(boost::property_tree::ptree& pt, std::vector<PatternCard>* frameData, int index, int subIndex, int& subSubIndex) {

	if (pt.empty()) {
		if (subSubIndex == 5) {
			subSubIndex = 0;
		}
		std::string thing = pt.data();
		frameData->at(index).setIndex(thing, subIndex, subSubIndex);
		subSubIndex = subSubIndex + 1;
	}
	else {
		for (boost::property_tree::ptree::iterator pos = pt.begin(); pos != pt.end();) {
			fillFrameArray(pos->second, frameData, index, subIndex, subSubIndex);
			pos++;
		}
	}
}

int drawFrame(std::string frameName, std::vector<std::vector<Die>> frameBoard, int frameTokens, sf::RenderWindow* window, int position) {
	double ratio = 2.5;

	sf::ConvexShape rectBorder;
	if (position == 1) { // top-left
		rectBorder = RoundedRectangle(0 + SCREEN_WIDTH / (ratio * 8), 0 + SCREEN_HEIGHT / (ratio * 8),
			SCREEN_WIDTH / ratio, SCREEN_HEIGHT / ratio, 10, sf::Color::White, 2, sf::Color::Black);
	}
	else if (position == 2) {
		rectBorder = RoundedRectangle(SCREEN_WIDTH / 2.0 + SCREEN_WIDTH / (ratio * 8), 0 + SCREEN_HEIGHT / (ratio * 8),
			SCREEN_WIDTH / ratio, SCREEN_HEIGHT / ratio, 10, sf::Color::White, 2, sf::Color::Black);
	}
	else if (position == 3) {
		rectBorder = RoundedRectangle(0 + SCREEN_WIDTH / (ratio * 8), SCREEN_HEIGHT / 2.0 + SCREEN_HEIGHT / (ratio * 8),
			SCREEN_WIDTH / ratio, SCREEN_HEIGHT / ratio, 10, sf::Color::White, 2, sf::Color::Black);
	}
	else if (position == 4) {
		rectBorder = RoundedRectangle(SCREEN_WIDTH / 2.0 + SCREEN_WIDTH / (ratio * 8), SCREEN_HEIGHT / 2.0 + SCREEN_HEIGHT / (ratio * 8),
			SCREEN_WIDTH / ratio, SCREEN_HEIGHT / ratio, 10, sf::Color::White, 2, sf::Color::Black);
	}
	else {
		return 0;
	}

	rectBorder.setFillColor(darkGray);
	sf::FloatRect shapeBounds = rectBorder.getLocalBounds();

	sf::Font font;
	font.loadFromFile("arial.ttf");
	sf::Text frameNameText;
	frameNameText.setFont(font);
	frameNameText.setString(frameName);
	frameNameText.setCharacterSize(24.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
	frameNameText.setFillColor(sf::Color::Black);
	frameNameText.setStyle(sf::Text::Bold);

	sf::FloatRect textRect = frameNameText.getLocalBounds();
	frameNameText.setOrigin(0, textRect.height);

	frameNameText.setPosition(shapeBounds.left + SCREEN_WIDTH / margin, shapeBounds.top + shapeBounds.height - SCREEN_HEIGHT / margin * 1.75);

	sf::Text frameTokensText;
	frameTokensText = frameNameText;
	frameTokensText.setString(std::to_string(frameTokens));
	textRect = frameTokensText.getLocalBounds();
	frameTokensText.setOrigin(textRect.width, textRect.height);
	frameTokensText.setPosition(shapeBounds.left + shapeBounds.width - SCREEN_WIDTH / margin, shapeBounds.top + shapeBounds.height - SCREEN_HEIGHT / margin * 1.8);

	if (MouseInConvexShape(rectBorder, window)) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			return position;
		}
		rectBorder.setOutlineColor(sf::Color::Yellow);
	}

	// just use the rects for proper positioning, don't actually draw them
	window->draw(rectBorder);
	window->draw(frameNameText);
	window->draw(frameTokensText);
	drawBoard(frameBoard, window, rectBorder, Die());
	return 0;
}

std::vector<int> drawBoard(std::vector<std::vector<Die>> frameBoard, sf::RenderWindow* window, sf::ConvexShape border, Die selectedDie) {
	std::vector<std::vector<bool>> clickable = determinePlacementSpots(frameBoard, selectedDie, "None");
	float squareSize = border.getGlobalBounds().height / ((frameBoard.size() + 1.0) * 1.25);
	sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
	double startX = border.getGlobalBounds().left + SCREEN_WIDTH / margin * 2.5;
	double startY = border.getGlobalBounds().top + SCREEN_HEIGHT / margin * 1.75;
	double newX = startX;
	double newY = startY;
	for (int i = 0; i < frameBoard.size(); i++) {
		for (int j = 0; j < frameBoard[0].size(); j++) {
			sf::Color outline = mediumGray;
			if (clickable[i][j]) {
				outline = sf::Color::Green;
			}
			// draw die and check if it has been selected to place a dice from the draft pool in
			if (drawDie(frameBoard[i][j], squareSize, startX + (squareSize + SCREEN_WIDTH / margin) * j,
				startY + (squareSize + SCREEN_HEIGHT / margin) * i, outline, selectedDie, window).getFillColor() == sf::Color::White) {
				std::vector<int> chosenDie = { i, j };
				return chosenDie;
				}
		}
	}
	return std::vector<int>();
}

sf::RectangleShape drawDie(Die currentDie, float size, float x, float y, sf::Color isClickableOutlineColor, Die selectedDie, sf::RenderWindow* window) {
	sf::RectangleShape square(sf::Vector2f(size, size));
	square.setPosition(x, y);
	square.setOutlineColor(mediumGray);
	square.setOutlineThickness(2);
	square.setFillColor(lightGray);
	// Set fill color, draw dots
	if (currentDie.getColor() == "Blue") {
		square.setFillColor(blue);
	}
	else if (currentDie.getColor() == "Red") {
		square.setFillColor(red);
	}
	else if (currentDie.getColor() == "Green") {
		square.setFillColor(green);
	}
	else if (currentDie.getColor() == "Purple") {
		square.setFillColor(purple);
	}
	else if (currentDie.getColor() == "Yellow") {
		square.setFillColor(yellow);
	}
	sf::CircleShape dotCircle;
	dotCircle.setRadius(square.getGlobalBounds().height / 10.0);
	dotCircle.setFillColor(sf::Color::Black);
	float dotMargin = square.getSize().x / 18.f;
	if (currentDie.getNumber() != 0 && currentDie.getColor() == "") {
		square.setFillColor(sf::Color(74, 79, 78, 255));
	}
	square.setOutlineColor(isClickableOutlineColor);
	if (square.getGlobalBounds().contains((sf::Vector2f)sf::Mouse::getPosition(*window)) && selectedDie.isFull()) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && isClickableOutlineColor == sf::Color::Green) { // click draft pool die
			square.setFillColor(sf::Color::White); // the die has been clicked!
		}
		// color on hover
		square.setOutlineColor(sf::Color::Yellow);
	}
	window->draw(square);
	if ((std::set<int> {2, 4, 5, 6}).count(currentDie.getNumber()) > 0) { // top-left and bottom-right
		dotCircle.setPosition(square.getPosition().x + dotMargin,
			square.getPosition().y + dotMargin);
		window->draw(dotCircle); // top-left
		dotCircle.setPosition(square.getPosition().x + square.getSize().x - dotCircle.getRadius() * 2.0 - dotMargin,
			square.getPosition().y + square.getSize().y - dotCircle.getRadius() * 2.0 - dotMargin);
		window->draw(dotCircle); // bottom-right
	}
	if ((std::set<int> {1, 3, 5}).count(currentDie.getNumber()) > 0) { // center
		dotCircle.setPosition(square.getPosition().x + (square.getSize().x - dotCircle.getRadius() * 2.0) / 2.f,
			square.getPosition().y + (square.getSize().y - dotCircle.getRadius() * 2.0) / 2.f);
		window->draw(dotCircle); // center
	}
	if ((std::set<int> {3, 4, 5, 6}).count(currentDie.getNumber()) > 0) { // top-right and bottom-left
		dotCircle.setPosition(square.getPosition().x + square.getSize().x - dotCircle.getRadius() * 2.0 - dotMargin,
			square.getPosition().y + dotMargin);
		window->draw(dotCircle); // top-right
		dotCircle.setPosition(square.getPosition().x + dotMargin,
			square.getPosition().y + square.getSize().y - dotCircle.getRadius() * 2.0 - dotMargin);
		window->draw(dotCircle); // bottom-left
	}
	if ((std::set<int> {6}).count(currentDie.getNumber()) > 0) { // middle-left and middle-right
		dotCircle.setPosition(square.getPosition().x + dotMargin,
			square.getPosition().y + (square.getSize().y - dotCircle.getRadius() * 2.0) / 2.f);
		window->draw(dotCircle); // middle-left
		dotCircle.setPosition(square.getPosition().x + square.getSize().x - dotCircle.getRadius() * 2.0 - dotMargin,
			square.getPosition().y + (square.getSize().y - dotCircle.getRadius() * 2.0) / 2.f);
		window->draw(dotCircle); // middle-right
	}
	return square;
}

int scoreBoard(PlayerBoard board, std::vector<PublicObjective> selectedPublicObjectives, std::string privateObjective) {
	int pointTotal = 0;
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
					if (numbers.count(num) > 0) {
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
					if (colors.count(color) > 0) {
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
					if (numbers.count(num) > 0) {
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
					if (colors.count(color) > 0) {
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
			counts.push_back(board.countColor("Red"));
			pointTotal += *std::max_element(counts.begin(), counts.end()) * std::stoi(selectedPublicObjectives.at(j).getPoints());
		}
		else if (selectedPublicObjectives.at(j).getName() == "Color Diagonals") {
			// Count of diagonally adjacent same-color dice
			int count = 0;
			std::vector<bool> alreadyCounted(20, false);
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 5; ++j) {
					std::string color = board.getDice()[i][j].getColor();
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
	}
	// add up private objective
	pointTotal += board.countColorTotal(privateObjective);
	// lose one point for each empty space
	pointTotal -= board.countEmpty();
	// add one point for each remaining favor token
	// (removed due to no tool card functionality)
	// players.at(i).addPoints(players.at(i).getTokens());
	return pointTotal;
}
