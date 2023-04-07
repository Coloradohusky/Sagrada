#include <SFML/Graphics.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <random>
#include <string>
#include <Windows.h>
#include <set>
#include "CustomShapes.hpp"
#include "DiceBag.hpp"
#include "DicePool.hpp"
#include "Die.hpp"
#include "PatternCard.hpp"
#include "Player.hpp"
#include "PlayerBoard.hpp"
#include "RoundTrack.hpp"

const int DEFAULT_SCREEN_WIDTH = 1120;
int *SCREEN_WIDTH;
int *SCREEN_HEIGHT;
// 1920x1080 = 16:9
// 240:127 = fullscreen on 16:9
const double ratio = 9.0 / 16.0;
const int sleepTime = 200;
const double margin = 80.0;
const sf::Color darkGray(145, 150, 150, 255);
const sf::Color gray(146, 153, 152, 255);
const sf::Color beige(253, 217, 150, 255);

int main();

std::vector<std::vector<bool>> determinePlacementSpots(std::vector<std::vector<Die>> board, Die selectedDie, std::string restrictions);

void fillFrameArray(boost::property_tree::ptree& pt, std::vector<PatternCard>* frameData, int index, int subIndex, int& subSubIndex);

int drawFrame(std::string frameName, std::vector<std::vector<Die>> frameBoard, int frameTokens, sf::RenderWindow* window, int position);

int drawBoard(std::vector<std::vector<Die>> frameBoard, sf::RenderWindow* window, sf::ConvexShape border, Die selectedDie);

int main() {
	SCREEN_WIDTH = new int(1120);
	SCREEN_HEIGHT = new int(*SCREEN_WIDTH * ratio);
	int playerCount = 0;

	sf::Font font;
	font.loadFromFile("arial.ttf");

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(*SCREEN_WIDTH, *SCREEN_HEIGHT), " Sagrada", sf::Style::Default, settings);
	sf::Image icon;
	icon.loadFromFile("Icon.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	sf::RectangleShape background(sf::Vector2f(*SCREEN_WIDTH, *SCREEN_HEIGHT));
	background.setFillColor(gray);

	boost::property_tree::ptree windowFrames;
	read_json("windowFrames.json", windowFrames);
	windowFrames.erase("Blank");

	boost::property_tree::ptree publicObjectives;
	read_json("publicObjectives.json", publicObjectives);
	publicObjectives.erase("Blank");

	boost::property_tree::ptree toolCards;
	read_json("toolCards.json", toolCards);
	toolCards.erase("Blank");

	std::vector<Player> players = { Player(1), Player(2), Player(3), Player(4) };
	std::vector<std::string> private_colors = {"Blue", "Red", "Green", "Purple", "Yellow"};
	std::shuffle(std::begin(private_colors), std::end(private_colors), std::mt19937(static_cast<uint32_t>(time(0))));
	for (int i = 0; i < players.size(); i++) {
		players[i].setPrivateObjective(private_colors[0]);
		private_colors.erase(private_colors.begin());
	}

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0.0, 12.0); // 0 to 11
	std::vector<PatternCard> selectedPatternCards;

	DicePool dicePool;
	DiceBag diceBag;
	int currentPlayer = 1;
	int currentTurn = 1;
	int playGame = 0;

	for (int i = 0; i < 8; i++) {
		int randomNumber = dist(mt);
		while (windowFrames.find(std::to_string(randomNumber)) == windowFrames.not_found()) {
			randomNumber = dist(mt);
		}
		for (int j = 0; j < 2; j++) {
			selectedPatternCards.insert(selectedPatternCards.end(), PatternCard());
			selectedPatternCards.back().setName(windowFrames.get_child(std::to_string(randomNumber)).front().first);
			selectedPatternCards.back().setTokens(std::stoi(windowFrames.get_child(std::to_string(randomNumber)).front().second.get<std::string>("Tokens")));
			for (int k = 0; k < 4; k++) {
				boost::property_tree::ptree temp = windowFrames.get_child(std::to_string(randomNumber)).front().second.get_child("Board").begin()->second;
				int subSubIndex = 0;
				fillFrameArray(temp, &selectedPatternCards, (i * 2) + j, k, subSubIndex);
				windowFrames.get_child(std::to_string(randomNumber)).front().second.get_child("Board").pop_front();
			}
			windowFrames.get_child(std::to_string(randomNumber)).pop_front();
		}
		windowFrames.erase(std::to_string(randomNumber));
	}

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
			if (event.type == sf::Event::Resized) {
				*SCREEN_WIDTH = window.getSize().x;
				*SCREEN_HEIGHT = *SCREEN_WIDTH * ratio; // 16:10 aspect ratio
				window.setSize(sf::Vector2u(*SCREEN_WIDTH, *SCREEN_HEIGHT));
				window.setView(sf::View(sf::FloatRect(0.0, 0.0, *SCREEN_WIDTH, *SCREEN_HEIGHT)));
				background.setSize(sf::Vector2f(*SCREEN_WIDTH, *SCREEN_HEIGHT));
			}
        }
		window.setSize(sf::Vector2u(*SCREEN_WIDTH, *SCREEN_HEIGHT));
        window.clear();
		window.draw(background);
		if (playerCount == 0) {
			sf::Text playerText[3];
			playerText[0].setFont(font);
			playerText[0].setString("players");
			playerText[0].setCharacterSize(24.0 * ((double)*SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
			playerText[0].setFillColor(sf::Color::Black);
			playerText[0].setStyle(sf::Text::Bold);
			playerText[1] = playerText[0];
			playerText[2] = playerText[0];
			sf::Text otherPlayerText[3];
			otherPlayerText[0].setFont(font);
			otherPlayerText[0].setCharacterSize(125.0 * ((double)*SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
			otherPlayerText[0].setFillColor(sf::Color::Black);
			otherPlayerText[0].setStyle(sf::Text::Bold);
			otherPlayerText[1] = otherPlayerText[0];
			otherPlayerText[2] = otherPlayerText[0];
			int buttonWidth = *SCREEN_WIDTH / 5;
			int buttonHeight = *SCREEN_HEIGHT / 3.125;
			int buttonMargin = *SCREEN_WIDTH / 50;
			int middleButtonXCoord = (*SCREEN_WIDTH / 2) - (buttonWidth / 2);
			sf::ConvexShape playerButtons[3];
			playerButtons[0] = RoundedRectangle(middleButtonXCoord - buttonWidth - buttonMargin, *SCREEN_HEIGHT / 1.8,
				buttonWidth, buttonHeight, 10,
				darkGray, 2, sf::Color::Black);
			playerButtons[1] = RoundedRectangle(middleButtonXCoord, *SCREEN_HEIGHT / 1.8,
				buttonWidth, buttonHeight, 10,
				darkGray, 2, sf::Color::Black);
			playerButtons[2] = RoundedRectangle(middleButtonXCoord + buttonWidth + buttonMargin, *SCREEN_HEIGHT / 1.8,
				buttonWidth, buttonHeight, 10,
				darkGray, 2, sf::Color::Black);
			for (int i = 0; i < 3; i++) {
				sf::FloatRect textBounds = playerText[i].getLocalBounds();
				sf::FloatRect shapeBounds = playerButtons[i].getLocalBounds();
				float bottom = shapeBounds.top + shapeBounds.height * 4 / 5;
				playerText[i].setPosition(shapeBounds.left + shapeBounds.width / 2 - textBounds.width / 2, bottom - textBounds.height / 2);
				otherPlayerText[i].setString(std::to_string(i + 2));
				sf::FloatRect otherTextBounds = otherPlayerText[i].getLocalBounds();
				otherPlayerText[i].setPosition(shapeBounds.left + shapeBounds.width / 2 - otherTextBounds.width / 2, shapeBounds.top + shapeBounds.height / 4 - otherTextBounds.height / 2);
			}
			sf::Texture* logoTexture = new sf::Texture();
			sf::RectangleShape logoRect;
			// SagradaLogo.jpg: 960x540 (16:9)
			int logoWidth = *SCREEN_WIDTH * (5.0 / 12.0);
			int logoHeight = logoWidth * (9.0 / 16.0);
			logoRect.setSize(sf::Vector2f(logoWidth, logoHeight));
			logoRect.setPosition(sf::Vector2f((*SCREEN_WIDTH / 2) - (logoWidth / 2), (*SCREEN_HEIGHT / 8) - (logoHeight / 8)));
			logoTexture->loadFromFile("SagradaLogo.png");
			logoRect.setTexture(logoTexture);
			playerButtons[0].setOutlineColor(sf::Color::Black);
			playerButtons[1].setOutlineColor(sf::Color::Black);
			playerButtons[2].setOutlineColor(sf::Color::Black);
			if (MouseInConvexShape(playerButtons[0], &window)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					playerCount = 2;
					Sleep(sleepTime);
				}
				playerButtons[0].setOutlineColor(sf::Color::Yellow);
			}
			if (MouseInConvexShape(playerButtons[1], &window)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					playerCount = 3;
					Sleep(sleepTime);
				}
				playerButtons[1].setOutlineColor(sf::Color::Yellow);
			}
			if (MouseInConvexShape(playerButtons[2], &window)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					playerCount = 4;
					Sleep(sleepTime);
				}
				playerButtons[2].setOutlineColor(sf::Color::Yellow);
			}
			window.draw(playerButtons[0]);
			window.draw(playerButtons[1]);
			window.draw(playerButtons[2]);
			window.draw(playerText[0]);
			window.draw(playerText[1]);
			window.draw(playerText[2]);
			window.draw(otherPlayerText[0]);
			window.draw(otherPlayerText[1]);
			window.draw(otherPlayerText[2]);
			window.draw(logoRect);
		}
		else if (playGame != playerCount - 1) {
			while (players.size() != playerCount) { // remove extra players if needed
				players.pop_back();
			}
			for (int i = 0; i < playerCount; i++) {
				if (players[i].boardIsEmpty()) {
					int selected = 0;
					selected = selected + drawFrame(selectedPatternCards[i * 4 + 0].getName(), selectedPatternCards[i * 4 + 0].getDice(), selectedPatternCards[i * 4 + 0].getTokens(), &window, 1);
					selected = selected + drawFrame(selectedPatternCards[i * 4 + 1].getName(), selectedPatternCards[i * 4 + 1].getDice(), selectedPatternCards[i * 4 + 1].getTokens(), &window, 2);
					selected = selected + drawFrame(selectedPatternCards[i * 4 + 2].getName(), selectedPatternCards[i * 4 + 2].getDice(), selectedPatternCards[i * 4 + 2].getTokens(), &window, 3);
					selected = selected + drawFrame(selectedPatternCards[i * 4 + 3].getName(), selectedPatternCards[i * 4 + 3].getDice(), selectedPatternCards[i * 4 + 3].getTokens(), &window, 4);
					sf::Text header;
					header.setFont(font);
					header.setString("P" + std::to_string(i + 1));
					header.setCharacterSize(30.0 * ((double)*SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
					header.setFillColor(sf::Color::Black);
					header.setStyle(sf::Text::Bold);
					header.setOrigin(header.getLocalBounds().width / 2, header.getLocalBounds().height / 2);
					header.setPosition(*SCREEN_WIDTH / 2.0, *SCREEN_HEIGHT / 2.0);
					window.draw(header);
					sf::RectangleShape privColor;
					privColor.setSize(sf::Vector2f(*SCREEN_WIDTH / 16.0, *SCREEN_HEIGHT / 8.0));
					privColor.setOrigin(sf::Vector2f(*SCREEN_WIDTH / 32.0, *SCREEN_HEIGHT / 16.0));
					privColor.setOutlineThickness(1);
					privColor.setOutlineColor(sf::Color::Black);
					privColor.setPosition(*SCREEN_WIDTH / 2.0, *SCREEN_HEIGHT / 1.5);
					if (players[i].getPrivateObjective() == "Blue") {
						privColor.setFillColor(sf::Color(45, 187, 200, 255));
					}
					else if (players[i].getPrivateObjective() == "Red") {
						privColor.setFillColor(sf::Color(220, 35, 39, 255));
					}
					else if (players[i].getPrivateObjective() == "Green") {
						privColor.setFillColor(sf::Color(3, 171, 108, 255));
					}
					else if (players[i].getPrivateObjective() == "Purple") {
						privColor.setFillColor(sf::Color(165, 65, 152, 255));
					}
					else if (players[i].getPrivateObjective() == "Yellow") {
						privColor.setFillColor(sf::Color(243, 222, 12, 255));
					}
					window.draw(privColor);
					if (selected != 0) {
						players[i].setFrame(selectedPatternCards[i * 4 + selected - 1].getDice());
						players[i].setTokens(selectedPatternCards[i * 4 + selected - 1].getTokens());
						Sleep(sleepTime);
					}
					else {
						break;
					}
					playGame = i;
				}
			}
		}
		else {
			if (dicePool.isEmpty()) {
				dicePool.roll(&diceBag, playerCount);
			}
			int currentPlayerIsDone = 0;
			if (currentPlayerIsDone == 1) {
				if (currentTurn == 10 && currentPlayer == playerCount) {
					// the game is over
					window.draw(sf::RectangleShape());
					break;
				} else if (currentPlayer == playerCount) {
					currentPlayer = 1;
					currentTurn++;
				}
				else {
					currentPlayer++;
				}
				currentPlayerIsDone = 0;
			}
			else {
				sf::ConvexShape boardOutline;
				float percentage = (float)*SCREEN_WIDTH / (float)DEFAULT_SCREEN_WIDTH;
				float margin = 20.0 * percentage;
				float boardHeight = 400.0 * percentage;
				float boardWidth = 450.0 * percentage;
				boardOutline.setOrigin(boardWidth, 0);
				boardOutline = RoundedRectangle(*SCREEN_WIDTH - margin - boardWidth, 0 + margin,
					boardWidth, boardHeight, 10, darkGray, 2, sf::Color::Black);
				window.draw(boardOutline);
				drawBoard(players[currentPlayer - 1].getBoard().getDice(), &window, boardOutline, Die());
				sf::RectangleShape privObj;
				if (players[currentPlayer - 1].getPrivateObjective() == "Blue") {
					privObj.setFillColor(sf::Color(45, 187, 200, 255));
				}
				else if (players[currentPlayer - 1].getPrivateObjective() == "Red") {
					privObj.setFillColor(sf::Color(220, 35, 39, 255));
				}
				else if (players[currentPlayer - 1].getPrivateObjective() == "Green") {
					privObj.setFillColor(sf::Color(3, 171, 108, 255));
				}
				else if (players[currentPlayer - 1].getPrivateObjective() == "Purple") {
					privObj.setFillColor(sf::Color(165, 65, 152, 255));
				}
				else if (players[currentPlayer - 1].getPrivateObjective() == "Yellow") {
					privObj.setFillColor(sf::Color(243, 222, 12, 255));
				}
				privObj.setSize(sf::Vector2f(boardWidth / 1.5, boardWidth / 9.0));
				privObj.setPosition(*SCREEN_WIDTH - margin * 1.7 - boardWidth / 1.5, 0 - margin * 2.75 + boardHeight);
				privObj.setOutlineThickness(1);
				privObj.setOutlineColor(sf::Color::Black);
				window.draw(privObj);
				sf::Text tokenCount;
				tokenCount.setString(std::to_string(players[currentPlayer - 1].getTokens()) + "T");
				tokenCount.setFont(font);
				tokenCount.setStyle(sf::Text::Bold);
				tokenCount.setFillColor(sf::Color::Black);
				tokenCount.setCharacterSize(40.0 * ((double)*SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
				tokenCount.setPosition(*SCREEN_WIDTH + margin - boardWidth, 0 - margin * 2.8 + boardHeight);
				window.draw(tokenCount);
				// draw token count and private objective in bottom of the board outline
				// objective + tool cards are 63x88
				sf::ConvexShape objectiveCards[3];
				float objectiveCardWidth = 63.0 * 2.05 * ((double)*SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				float objectiveCardHeight = 88.0 * 2.05 * ((double)*SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				objectiveCards[0] = RoundedRectangle(*SCREEN_WIDTH - 2 * margin - boardWidth - objectiveCardWidth, 0 + margin, objectiveCardWidth, objectiveCardHeight, 10, darkGray, 5, sf::Color::Black);
				objectiveCards[1] = RoundedRectangle(*SCREEN_WIDTH - 2 * margin - boardWidth - objectiveCardWidth, 0 + 2 * margin + objectiveCardHeight, objectiveCardWidth, objectiveCardHeight, 10, darkGray, 5, sf::Color::Black);
				objectiveCards[2] = RoundedRectangle(*SCREEN_WIDTH - 2 * margin - boardWidth - objectiveCardWidth, 0 + 3 * margin + 2 * objectiveCardHeight, objectiveCardWidth, objectiveCardHeight, 10, darkGray, 5, sf::Color::Black);
				sf::ConvexShape toolCards[3];
				toolCards[0] = RoundedRectangle(*SCREEN_WIDTH - 3 * margin - boardWidth - 2 * objectiveCardWidth, 0 + margin, objectiveCardWidth, objectiveCardHeight, 10, beige, 5, sf::Color::Black);
				toolCards[1] = RoundedRectangle(*SCREEN_WIDTH - 3 * margin - boardWidth - 2 * objectiveCardWidth, 0 + 2 * margin + objectiveCardHeight, objectiveCardWidth, objectiveCardHeight, 10, beige, 5, sf::Color::Black);
				toolCards[2] = RoundedRectangle(*SCREEN_WIDTH - 3 * margin - boardWidth - 2 * objectiveCardWidth, 0 + 3 * margin + 2 * objectiveCardHeight, objectiveCardWidth, objectiveCardHeight, 10, beige, 5, sf::Color::Black);
				window.draw(objectiveCards[0]);
				window.draw(objectiveCards[1]);
				window.draw(objectiveCards[2]);
				window.draw(toolCards[0]);
				window.draw(toolCards[1]);
				window.draw(toolCards[2]);
				sf::ConvexShape draftPool;
				float draftPoolWidth = 300.0 * ((double)*SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				float draftPoolHeight = 583.0 * ((double)*SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				draftPool = RoundedRectangle(0 + margin, 0 + margin, draftPoolWidth, draftPoolHeight, 10, darkGray, 2, sf::Color::Black);
				//window.draw(draftPool);
				std::vector<sf::ConvexShape> dicePoolShapes;
				for (int d = 0; d < dicePool.size(); d++) {
					if (dicePoolShapes.size() != dicePool.size()) {
						float diceSize = 100.0 * ((double)*SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
						dicePoolShapes.push_back(RoundedRectangle(0 + margin + d * 6.0, 0 + margin + d * 6.0, diceSize, diceSize, 10, dicePool.getDieColor(d), 2, sf::Color::Black));
					}
					window.draw(dicePoolShapes[d]);
				}
			}
			// until max playercount is reached, then set back to 1 and increment currentTurn by 1
		}
        window.display();
    }
    return 0;
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
		rectBorder = RoundedRectangle(0 + *SCREEN_WIDTH / (ratio * 8), 0 + *SCREEN_HEIGHT / (ratio * 8),
			*SCREEN_WIDTH / ratio, *SCREEN_HEIGHT / ratio, 10, sf::Color::White, 2, sf::Color::Black);
	}
	else if (position == 2) {
		rectBorder = RoundedRectangle(*SCREEN_WIDTH / 2.0 + *SCREEN_WIDTH / (ratio * 8), 0 + *SCREEN_HEIGHT / (ratio * 8),
			*SCREEN_WIDTH / ratio, *SCREEN_HEIGHT / ratio, 10, sf::Color::White, 2, sf::Color::Black);
	}
	else if (position == 3) {
		rectBorder = RoundedRectangle(0 + *SCREEN_WIDTH / (ratio * 8), *SCREEN_HEIGHT / 2.0 + *SCREEN_HEIGHT / (ratio * 8),
			*SCREEN_WIDTH / ratio, *SCREEN_HEIGHT / ratio, 10, sf::Color::White, 2, sf::Color::Black);
	}
	else if (position == 4) {
		rectBorder = RoundedRectangle(*SCREEN_WIDTH / 2.0 + *SCREEN_WIDTH / (ratio * 8), *SCREEN_HEIGHT / 2.0 + *SCREEN_HEIGHT / (ratio * 8),
			*SCREEN_WIDTH / ratio, *SCREEN_HEIGHT / ratio, 10, sf::Color::White, 2, sf::Color::Black);
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
	frameNameText.setCharacterSize(24.0 * ((double)*SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
	frameNameText.setFillColor(sf::Color::Black);
	frameNameText.setStyle(sf::Text::Bold);
	
	sf::FloatRect textRect = frameNameText.getLocalBounds();
	frameNameText.setOrigin(0, textRect.height);
	
	frameNameText.setPosition(shapeBounds.left + *SCREEN_WIDTH / margin, shapeBounds.top + shapeBounds.height - *SCREEN_HEIGHT / margin * 1.75);

	sf::Text frameTokensText;
	frameTokensText = frameNameText;
	frameTokensText.setString(std::to_string(frameTokens));
	textRect = frameTokensText.getLocalBounds();
	frameTokensText.setOrigin(textRect.width, textRect.height);
	frameTokensText.setPosition(shapeBounds.left + shapeBounds.width - *SCREEN_WIDTH / margin, shapeBounds.top + shapeBounds.height - *SCREEN_HEIGHT / margin * 1.8);

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
	double startX = border.getGlobalBounds().left + *SCREEN_WIDTH / margin * 2.5;
	double startY = border.getGlobalBounds().top + *SCREEN_HEIGHT / margin * 1.75;
	double newX = startX;
	double newY = startY;
	for (int i = 0; i < frameBoard.size(); i++) {
		for (int j = 0; j < frameBoard[0].size(); j++) {
			sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
			square.setPosition(startX + (squareSize + *SCREEN_WIDTH / margin) * j, startY + (squareSize + *SCREEN_HEIGHT / margin) * i);
			square.setOutlineColor(sf::Color(99, 105, 104, 255));
			square.setOutlineThickness(2);
			square.setFillColor(sf::Color(202, 206, 205, 255));
			// Set fill color, draw dots
			if (frameBoard[i][j].getColor() == "Blue") {
				square.setFillColor(sf::Color(45, 187, 200, 255));
			}
			else if (frameBoard[i][j].getColor() == "Red") {
				square.setFillColor(sf::Color(220, 35, 39, 255));
			}
			else if (frameBoard[i][j].getColor() == "Green") {
				square.setFillColor(sf::Color(3, 171, 108, 255));
			}
			else if (frameBoard[i][j].getColor() == "Purple") {
				square.setFillColor(sf::Color(165, 65, 152, 255));
			}
			else if (frameBoard[i][j].getColor() == "Yellow") {
				square.setFillColor(sf::Color(243, 222, 12, 255));
			}
			sf::CircleShape dotCircle;
			dotCircle.setRadius(square.getGlobalBounds().height / 10.0);
			dotCircle.setFillColor(sf::Color::Black);
			float dotMargin = square.getSize().x / 18.f;
			if (frameBoard[i][j].getNumber() != 0 && frameBoard[i][j].getColor() == "") {
				square.setFillColor(sf::Color(74, 79, 78, 255));
			}
			if (clickable[i][j] == true) {
				square.setOutlineColor(sf::Color::Yellow);
			}
			window->draw(square);
			if ((std::set<int> {2, 4, 5, 6}).count(frameBoard[i][j].getNumber()) > 0) { // top-left and bottom-right
				dotCircle.setPosition(square.getPosition().x + dotMargin,
					square.getPosition().y + dotMargin);
				window->draw(dotCircle); // top-left
				dotCircle.setPosition(square.getPosition().x + square.getSize().x - dotCircle.getRadius() * 2.0 - dotMargin,
					square.getPosition().y + square.getSize().y - dotCircle.getRadius() * 2.0 - dotMargin);
				window->draw(dotCircle); // bottom-right
			}
			if ((std::set<int> {1, 3, 5}).count(frameBoard[i][j].getNumber()) > 0) { // center
				dotCircle.setPosition(square.getPosition().x + (square.getSize().x - dotCircle.getRadius() * 2.0) / 2.f,
					square.getPosition().y + (square.getSize().y - dotCircle.getRadius() * 2.0) / 2.f);
				window->draw(dotCircle); // center
			}
			if ((std::set<int> {3, 4, 5, 6}).count(frameBoard[i][j].getNumber()) > 0) { // top-right and bottom-left
				dotCircle.setPosition(square.getPosition().x + square.getSize().x - dotCircle.getRadius() * 2.0 - dotMargin,
					square.getPosition().y + dotMargin);
				window->draw(dotCircle); // top-right
				dotCircle.setPosition(square.getPosition().x + dotMargin,
					square.getPosition().y + square.getSize().y - dotCircle.getRadius() * 2.0 - dotMargin);
				window->draw(dotCircle); // bottom-left
			}
			if ((std::set<int> {6}).count(frameBoard[i][j].getNumber()) > 0) { // middle-left and middle-right
				dotCircle.setPosition(square.getPosition().x + dotMargin,
					square.getPosition().y + (square.getSize().y - dotCircle.getRadius() * 2.0) / 2.f);
				window->draw(dotCircle); // middle-left
				dotCircle.setPosition(square.getPosition().x + square.getSize().x - dotCircle.getRadius() * 2.0 - dotMargin,
					square.getPosition().y + (square.getSize().y - dotCircle.getRadius() * 2.0) / 2.f);
				window->draw(dotCircle); // middle-right
			}
		}
	}
	return 0;
}