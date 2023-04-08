#include "Custom.hpp"

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
	return placementSpots;
}

void fillFrameArray(boost::property_tree::ptree& pt, std::vector<PatternCard>* frameData, int index, int subIndex, int& subSubIndex) {

	if (pt.empty()) {
		if (subSubIndex == 5) {
			subSubIndex = 0;
		}
		// std::vector<std::vector<std::vector<Die>>>& frameData
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

int drawFrame(std::string frameName, std::vector<std::vector<Die>> frameBoard, int frameTokens, sf::RenderWindow* window, int position)
{
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

	// just use the rects for proper positioning, don't actually draw them (unless...)
	window->draw(rectBorder);
	window->draw(frameNameText);
	window->draw(frameTokensText);
	drawBoard(frameBoard, window, rectBorder, Die());
	return 0;
}

int drawBoard(std::vector<std::vector<Die>> frameBoard, sf::RenderWindow* window, sf::ConvexShape border, Die selectedDie) {
	std::vector<std::vector<bool>> clickable = determinePlacementSpots(frameBoard, selectedDie, "None");
	float squareSize = border.getGlobalBounds().height / ((frameBoard.size() + 1.0) * 1.25);
	sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
	double startX = border.getGlobalBounds().left + SCREEN_WIDTH / margin * 2.5;
	double startY = border.getGlobalBounds().top + SCREEN_HEIGHT / margin * 1.75;
	double newX = startX;
	double newY = startY;
	for (int i = 0; i < frameBoard.size(); i++) {
		for (int j = 0; j < frameBoard[0].size(); j++) { // TODO: Turn into separate function (drawDie)
			drawDie(frameBoard[i][j], squareSize, startX + (squareSize + SCREEN_WIDTH / margin) * j, 
				startY + (squareSize + SCREEN_HEIGHT / margin) * i, clickable[i][j], window);
		}
	}
	return 0;
}

sf::RectangleShape drawDie(Die selectedDie, float size, float x, float y, bool isClickable, sf::RenderWindow* window) {
	sf::RectangleShape square(sf::Vector2f(size, size));
	square.setPosition(x, y);
	square.setOutlineColor(mediumGray);
	square.setOutlineThickness(2);
	square.setFillColor(lightGray); // TODO: Make global colors (add to Custom.hpp/cpp?)
	// Set fill color, draw dots
	if (selectedDie.getColor() == "Blue") {
		square.setFillColor(blue);
	}
	else if (selectedDie.getColor() == "Red") {
		square.setFillColor(red);
	}
	else if (selectedDie.getColor() == "Green") {
		square.setFillColor(green);
	}
	else if (selectedDie.getColor() == "Purple") {
		square.setFillColor(purple);
	}
	else if (selectedDie.getColor() == "Yellow") {
		square.setFillColor(yellow);
	}
	sf::CircleShape dotCircle;
	dotCircle.setRadius(square.getGlobalBounds().height / 10.0);
	dotCircle.setFillColor(sf::Color::Black);
	float dotMargin = square.getSize().x / 18.f;
	if (selectedDie.getNumber() != 0 && selectedDie.getColor() == "") {
		square.setFillColor(sf::Color(74, 79, 78, 255));
	}
	if (isClickable) {
		square.setOutlineColor(sf::Color::Yellow);
	}
	window->draw(square);
	if ((std::set<int> {2, 4, 5, 6}).count(selectedDie.getNumber()) > 0) { // top-left and bottom-right
		dotCircle.setPosition(square.getPosition().x + dotMargin,
			square.getPosition().y + dotMargin);
		window->draw(dotCircle); // top-left
		dotCircle.setPosition(square.getPosition().x + square.getSize().x - dotCircle.getRadius() * 2.0 - dotMargin,
			square.getPosition().y + square.getSize().y - dotCircle.getRadius() * 2.0 - dotMargin);
		window->draw(dotCircle); // bottom-right
	}
	if ((std::set<int> {1, 3, 5}).count(selectedDie.getNumber()) > 0) { // center
		dotCircle.setPosition(square.getPosition().x + (square.getSize().x - dotCircle.getRadius() * 2.0) / 2.f,
			square.getPosition().y + (square.getSize().y - dotCircle.getRadius() * 2.0) / 2.f);
		window->draw(dotCircle); // center
	}
	if ((std::set<int> {3, 4, 5, 6}).count(selectedDie.getNumber()) > 0) { // top-right and bottom-left
		dotCircle.setPosition(square.getPosition().x + square.getSize().x - dotCircle.getRadius() * 2.0 - dotMargin,
			square.getPosition().y + dotMargin);
		window->draw(dotCircle); // top-right
		dotCircle.setPosition(square.getPosition().x + dotMargin,
			square.getPosition().y + square.getSize().y - dotCircle.getRadius() * 2.0 - dotMargin);
		window->draw(dotCircle); // bottom-left
	}
	if ((std::set<int> {6}).count(selectedDie.getNumber()) > 0) { // middle-left and middle-right
		dotCircle.setPosition(square.getPosition().x + dotMargin,
			square.getPosition().y + (square.getSize().y - dotCircle.getRadius() * 2.0) / 2.f);
		window->draw(dotCircle); // middle-left
		dotCircle.setPosition(square.getPosition().x + square.getSize().x - dotCircle.getRadius() * 2.0 - dotMargin,
			square.getPosition().y + (square.getSize().y - dotCircle.getRadius() * 2.0) / 2.f);
		window->draw(dotCircle); // middle-right
	}
	return square;
}