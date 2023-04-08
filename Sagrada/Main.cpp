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
#include "Custom.hpp"
#include "DiceBag.hpp"
#include "DicePool.hpp"
#include "Die.hpp"
#include "PatternCard.hpp"
#include "Player.hpp"
#include "PlayerBoard.hpp"
#include "RoundTrack.hpp"

int SCREEN_WIDTH = 1120;
int SCREEN_HEIGHT = SCREEN_WIDTH * ratio;

int main();

int main() {
	int playerCount = 0;

	sf::Font font;
	font.loadFromFile("arial.ttf");

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), " Sagrada", sf::Style::Default, settings);
	sf::Image icon;
	icon.loadFromFile("Icon.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	sf::RectangleShape background(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
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
	int currentPlayerIsDone = 0;
	std::vector<int> playerOrder;

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
				SCREEN_WIDTH = window.getSize().x;
				SCREEN_HEIGHT = SCREEN_WIDTH * ratio; // 16:10 aspect ratio
				window.setSize(sf::Vector2u(SCREEN_WIDTH, SCREEN_HEIGHT));
				window.setView(sf::View(sf::FloatRect(0.0, 0.0, SCREEN_WIDTH, SCREEN_HEIGHT)));
				background.setSize(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
			}
        }
		window.setSize(sf::Vector2u(SCREEN_WIDTH, SCREEN_HEIGHT));
        window.clear();
		window.draw(background);
		if (playerCount == 0) {
			sf::Text playerText[3];
			playerText[0].setFont(font);
			playerText[0].setString("players");
			playerText[0].setCharacterSize(24.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
			playerText[0].setFillColor(sf::Color::Black);
			playerText[0].setStyle(sf::Text::Bold);
			playerText[1] = playerText[0];
			playerText[2] = playerText[0];
			sf::Text otherPlayerText[3];
			otherPlayerText[0].setFont(font);
			otherPlayerText[0].setCharacterSize(125.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
			otherPlayerText[0].setFillColor(sf::Color::Black);
			otherPlayerText[0].setStyle(sf::Text::Bold);
			otherPlayerText[1] = otherPlayerText[0];
			otherPlayerText[2] = otherPlayerText[0];
			int buttonWidth = SCREEN_WIDTH / 5;
			int buttonHeight = SCREEN_HEIGHT / 3.125;
			int buttonMargin = SCREEN_WIDTH / 50;
			int middleButtonXCoord = (SCREEN_WIDTH / 2) - (buttonWidth / 2);
			sf::ConvexShape playerButtons[3];
			playerButtons[0] = RoundedRectangle(middleButtonXCoord - buttonWidth - buttonMargin, SCREEN_HEIGHT / 1.8,
				buttonWidth, buttonHeight, 10,
				darkGray, 2, sf::Color::Black);
			playerButtons[1] = RoundedRectangle(middleButtonXCoord, SCREEN_HEIGHT / 1.8,
				buttonWidth, buttonHeight, 10,
				darkGray, 2, sf::Color::Black);
			playerButtons[2] = RoundedRectangle(middleButtonXCoord + buttonWidth + buttonMargin, SCREEN_HEIGHT / 1.8,
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
			int logoWidth = SCREEN_WIDTH * (5.0 / 12.0);
			int logoHeight = logoWidth * (9.0 / 16.0);
			logoRect.setSize(sf::Vector2f(logoWidth, logoHeight));
			logoRect.setPosition(sf::Vector2f((SCREEN_WIDTH / 2) - (logoWidth / 2), (SCREEN_HEIGHT / 8) - (logoHeight / 8)));
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
					header.setCharacterSize(30.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
					header.setFillColor(sf::Color::Black);
					header.setStyle(sf::Text::Bold);
					header.setOrigin(header.getLocalBounds().width / 2, header.getLocalBounds().height / 2);
					header.setPosition(SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0);
					window.draw(header);
					sf::RectangleShape privColor;
					privColor.setSize(sf::Vector2f(SCREEN_WIDTH / 16.0, SCREEN_HEIGHT / 8.0));
					privColor.setOrigin(sf::Vector2f(SCREEN_WIDTH / 32.0, SCREEN_HEIGHT / 16.0));
					privColor.setOutlineThickness(1);
					privColor.setOutlineColor(sf::Color::Black);
					privColor.setPosition(SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 1.5);
					if (players[i].getPrivateObjective() == "Blue") {
						privColor.setFillColor(blue);
					}
					else if (players[i].getPrivateObjective() == "Red") {
						privColor.setFillColor(red);
					}
					else if (players[i].getPrivateObjective() == "Green") {
						privColor.setFillColor(green);
					}
					else if (players[i].getPrivateObjective() == "Purple") {
						privColor.setFillColor(purple);
					}
					else if (players[i].getPrivateObjective() == "Yellow") {
						privColor.setFillColor(yellow);
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
			if (playerCount == 2) {
				playerOrder = { 1, 2, 2, 1, 0 }; // extra zero is so that we can detect when the round is over
			}
			else if (playerCount == 3) {
				playerOrder = { 1, 2, 3, 3, 2, 1, 0 };
			}
			else {
				playerOrder = { 1, 2, 3, 4, 4, 3, 2, 1, 0 };
			}
		}
		else {
			if (dicePool.isEmpty()) {
				dicePool.roll(&diceBag, playerCount);
			}
			if (currentPlayerIsDone == 1 || currentPlayer == 0) {
				if (currentTurn == 10 && currentPlayer == 0) {
					// the game is over
					window.draw(sf::RectangleShape());
					break;
				} else if (currentPlayer == 0) {
					if (playerOrder[1] == 1) {
						if (playerCount == 2) {
							playerOrder = { 2, 1, 1, 2, 0 };
						}
						else if (playerCount == 3) {
							playerOrder = { 2, 3, 1, 1, 3, 2, 0 };
						}
						else {
							playerOrder = { 2, 3, 4, 1, 1, 4, 3, 2, 0 };
						}
					}
					else if (playerOrder[1] == 2) {
						if (playerCount == 2) {
							playerOrder = { 1, 2, 2, 1, 0 };
						}
						else if (playerCount == 3) {
							playerOrder = { 3, 1, 2, 2, 1, 3, 0 };
						}
						else {
							playerOrder = { 3, 4, 1, 2, 2, 1, 4, 3, 0 };
						}
					}
					else if (playerOrder[1] == 3) {
						if (playerCount == 3) {
							playerOrder = { 1, 2, 3, 3, 2, 1, 0 };
						}
						else {
							playerOrder = { 4, 1, 2, 3, 3, 2, 1, 4, 0};
						}
					}
					else {
						playerOrder = { 1, 2, 3, 4, 4, 3, 2, 1, 0 };
					}
					currentPlayer = playerOrder[0];
					currentTurn++;
				}
				else {
					playerOrder.push_back(playerOrder[0]);
					playerOrder.erase(playerOrder.begin());
					currentPlayer = playerOrder[0];
				}
				currentPlayerIsDone = 0;
			}
			else if (currentPlayer != 0) {
				sf::ConvexShape boardOutline;
				float percentage = (float)SCREEN_WIDTH / (float)DEFAULT_SCREEN_WIDTH;
				float margin = 20.0 * percentage;
				float boardHeight = 400.0 * percentage;
				float boardWidth = 450.0 * percentage;
				boardOutline.setOrigin(boardWidth, 0);
				boardOutline = RoundedRectangle(SCREEN_WIDTH - margin - boardWidth, 0 + margin,
					boardWidth, boardHeight, 10, darkGray, 5, sf::Color::Black);
				window.draw(boardOutline);
				drawBoard(players[currentPlayer - 1].getBoard().getDice(), &window, boardOutline, Die());
				sf::RectangleShape privObj;
				if (players[currentPlayer - 1].getPrivateObjective() == "Blue") {
					privObj.setFillColor(blue);
				}
				else if (players[currentPlayer - 1].getPrivateObjective() == "Red") {
					privObj.setFillColor(red);
				}
				else if (players[currentPlayer - 1].getPrivateObjective() == "Green") {
					privObj.setFillColor(green);
				}
				else if (players[currentPlayer - 1].getPrivateObjective() == "Purple") {
					privObj.setFillColor(purple);
				}
				else if (players[currentPlayer - 1].getPrivateObjective() == "Yellow") {
					privObj.setFillColor(yellow);
				}
				privObj.setSize(sf::Vector2f(boardWidth / 1.5, boardWidth / 9.0));
				privObj.setPosition(SCREEN_WIDTH - margin * 1.7 - boardWidth / 1.5, 0 - margin * 2.75 + boardHeight);
				privObj.setOutlineThickness(1);
				privObj.setOutlineColor(sf::Color::Black);
				window.draw(privObj);
				sf::Text tokenCount;
				tokenCount.setString(std::to_string(players[currentPlayer - 1].getTokens()) + "T");
				tokenCount.setFont(font);
				tokenCount.setStyle(sf::Text::Bold);
				tokenCount.setFillColor(sf::Color::Black);
				tokenCount.setCharacterSize(40.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
				tokenCount.setPosition(SCREEN_WIDTH + margin - boardWidth, 0 - margin * 2.8 + boardHeight);
				window.draw(tokenCount);
				// draw token count and private objective in bottom of the board outline
				// objective + tool cards are 63x88
				sf::ConvexShape objectiveCards[3];
				float objectiveCardWidth = 63.0 * 2.05 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				float objectiveCardHeight = 88.0 * 2.05 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				objectiveCards[0] = RoundedRectangle(SCREEN_WIDTH - 2 * margin - boardWidth - objectiveCardWidth, 0 + margin, objectiveCardWidth, objectiveCardHeight, 10, darkGray, 5, sf::Color::Black);
				objectiveCards[1] = RoundedRectangle(SCREEN_WIDTH - 2 * margin - boardWidth - objectiveCardWidth, 0 + 2 * margin + objectiveCardHeight, objectiveCardWidth, objectiveCardHeight, 10, darkGray, 5, sf::Color::Black);
				objectiveCards[2] = RoundedRectangle(SCREEN_WIDTH - 2 * margin - boardWidth - objectiveCardWidth, 0 + 3 * margin + 2 * objectiveCardHeight, objectiveCardWidth, objectiveCardHeight, 10, darkGray, 5, sf::Color::Black);
				sf::ConvexShape toolCards[3];
				toolCards[0] = RoundedRectangle(SCREEN_WIDTH - 3 * margin - boardWidth - 2 * objectiveCardWidth, 0 + margin, objectiveCardWidth, objectiveCardHeight, 10, beige, 5, sf::Color::Black);
				toolCards[1] = RoundedRectangle(SCREEN_WIDTH - 3 * margin - boardWidth - 2 * objectiveCardWidth, 0 + 2 * margin + objectiveCardHeight, objectiveCardWidth, objectiveCardHeight, 10, beige, 5, sf::Color::Black);
				toolCards[2] = RoundedRectangle(SCREEN_WIDTH - 3 * margin - boardWidth - 2 * objectiveCardWidth, 0 + 3 * margin + 2 * objectiveCardHeight, objectiveCardWidth, objectiveCardHeight, 10, beige, 5, sf::Color::Black);
				window.draw(objectiveCards[0]);
				window.draw(objectiveCards[1]);
				window.draw(objectiveCards[2]);
				window.draw(toolCards[0]);
				window.draw(toolCards[1]);
				window.draw(toolCards[2]);
				sf::ConvexShape draftPool;
				float draftPoolWidth = 300.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				float draftPoolHeight = 583.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				draftPool = RoundedRectangle(0 + margin, 0 + margin, draftPoolWidth, draftPoolHeight, 10, darkGray, 5, sf::Color::Black);
				window.draw(draftPool);
				std::vector<sf::RectangleShape> dicePoolShapes;
				for (int d = 0; d < dicePool.size(); d++) {
					if (dicePoolShapes.size() != dicePool.size()) {
						float diceSize = 80.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
						float diceMargin = margin * 1.3;
						float diceX = 0 + diceMargin + d * 6.0; // TODO: RANDOMIZE within the borders of diceMargin
						float diceY = 0 + diceMargin + d * 6.0;
						// if clicked on or if == selectedDie, set selectedDie to that die AND set clickable to TRUE
						// otherwise, FALSE
						dicePoolShapes.push_back(drawDie(dicePool.getDie(d), diceSize, diceX, diceY, false, &window));
					}
				}
				//if (MouseInConvexShape(draftPool, &window)) {
				//	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				//		currentPlayerIsDone = 1;
				//	}
				//	draftPool.setOutlineColor(sf::Color::Yellow);
				//}
			}
			// until max playercount is reached, then set back to 1 and increment currentTurn by 1
		}
        window.display();
    }
    return 0;
}