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
#include "PublicObjective.hpp"
#include "RoundTrack.hpp"
#include "../SagradaTcpClient/SagradaTcpClient.hpp"

int main();

int main() {
	int playerCount = 0;
	int inMenu = 0;

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
	// Only done on host computer
	for (int i = 0; i < players.size(); i++) {
		players[i].setPrivateObjective(private_colors[0]);
		private_colors.erase(private_colors.begin());
	}

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> patternDist(0.0, 12.0); // 0 to 11
	std::vector<PatternCard> selectedPatternCards;
	std::uniform_real_distribution<double> objectiveDist(0.0, 10.0); // 0 to 9
	std::vector<PublicObjective> selectedPublicObjectives;

	DicePool dicePool;
	DiceBag diceBag;
	RoundTrack roundTrack;
	int currentPlayer = 1;
	int currentTurn = 1;
	int playGame = 0;
	int currentPlayerIsDone = 0;
	std::vector<int> playerOrder;
	std::vector<sf::RectangleShape> dicePoolShapes;
	int selectedDieIndex = -1;

	// This only needs to be done on host computer, then sent out to player computers
	for (int i = 0; i < 8; i++) {
		int randomNumber = patternDist(mt);
		while (windowFrames.find(std::to_string(randomNumber)) == windowFrames.not_found()) {
			randomNumber = patternDist(mt);
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

	// Same thing here - host only
	for (int i = 0; i < 3; i++) {
		int randomNumber = objectiveDist(mt);
		while (publicObjectives.find(std::to_string(randomNumber)) == publicObjectives.not_found()) {
			randomNumber = patternDist(mt);
		}
		selectedPublicObjectives.insert(selectedPublicObjectives.end(), PublicObjective());
		selectedPublicObjectives.back().setName(publicObjectives.get_child(std::to_string(randomNumber)).get<std::string>("Name"));
		selectedPublicObjectives.back().setDescription(publicObjectives.get_child(std::to_string(randomNumber)).get<std::string>("Description"));
		selectedPublicObjectives.back().setPoints(publicObjectives.get_child(std::to_string(randomNumber)).get<std::string>("Points"));
		publicObjectives.erase(std::to_string(randomNumber));
	}

	windowFrames.clear();
	publicObjectives.clear();
	toolCards.clear();

	// open client
	SagradaTcpClient client("25.49.77.220", 46580);
	std::thread tcpClient(&SagradaTcpClient::startServer, &client);

	// draw window loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
			if (event.type == sf::Event::Resized) {
				SCREEN_WIDTH = window.getSize().x;
				SCREEN_HEIGHT = SCREEN_WIDTH * ratio;
				window.setSize(sf::Vector2u(SCREEN_WIDTH, SCREEN_HEIGHT));
				window.setView(sf::View(sf::FloatRect(0.0, 0.0, SCREEN_WIDTH, SCREEN_HEIGHT)));
				background.setSize(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
			}
        }
		window.setSize(sf::Vector2u(SCREEN_WIDTH, SCREEN_HEIGHT));
        window.clear();
		window.draw(background);
		if (playerCount == 0 && inMenu == 0) { // start menu
			//*client.getOutBoundStream() << "(upd,)";
			//std::cout << "sent";
			sf::Text playerText[4];
			playerText[0].setFont(font);
			playerText[0].setString("players");
			playerText[0].setCharacterSize(24.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
			playerText[0].setFillColor(sf::Color::Black);
			playerText[0].setStyle(sf::Text::Bold);
			playerText[1] = playerText[0];
			playerText[2] = playerText[0];
			sf::Text otherPlayerText[4];
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
			sf::ConvexShape playerButtons[4];
			playerButtons[0] = RoundedRectangle(middleButtonXCoord - buttonWidth - buttonMargin, SCREEN_HEIGHT / 1.8,
				buttonWidth, buttonHeight, 10,
				darkGray, 2, sf::Color::Black);
			playerButtons[1] = RoundedRectangle(middleButtonXCoord, SCREEN_HEIGHT / 1.8,
				buttonWidth, buttonHeight, 10,
				darkGray, 2, sf::Color::Black);
			playerButtons[2] = RoundedRectangle(middleButtonXCoord + buttonWidth + buttonMargin, SCREEN_HEIGHT / 1.8,
				buttonWidth, buttonHeight, 10,
				darkGray, 2, sf::Color::Black);
			playerButtons[3] = RoundedRectangle(4, 4, buttonWidth / 1.5, buttonHeight / 3, 10,
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
			playerButtons[3].setOutlineColor(sf::Color::Black);

			playerText[3].setFont(font);
			playerText[3].setCharacterSize(50.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
			playerText[3].setFillColor(sf::Color::Black);
			playerText[3].setStyle(sf::Text::Bold);
			playerText[3].setString("Help?");
			sf::FloatRect textBounds = playerText[3].getLocalBounds();
			sf::FloatRect shapeBounds = playerButtons[3].getLocalBounds();
			playerText[3].setPosition(shapeBounds.left + shapeBounds.width / 2 - textBounds.width / 2, textBounds.height / 16);

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
			if (MouseInConvexShape(playerButtons[3], &window)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					inMenu = 1;
					Sleep(sleepTime);
				}
				playerButtons[3].setOutlineColor(sf::Color::Yellow);
			}
			window.draw(playerButtons[0]);
			window.draw(playerButtons[1]);
			window.draw(playerButtons[2]);
			window.draw(playerButtons[3]);
			window.draw(playerText[0]);
			window.draw(playerText[1]);
			window.draw(playerText[2]);
			window.draw(playerText[3]);
			window.draw(otherPlayerText[0]);
			window.draw(otherPlayerText[1]);
			window.draw(otherPlayerText[2]);
			window.draw(logoRect);
			delete logoTexture;
		}
		else if (inMenu == 1) { // help menu
			sf::Text menuText[2];
			sf::ConvexShape playerButton;
			sf::Text playerText;
			int buttonWidth = SCREEN_WIDTH / 7.5;
			int buttonHeight = SCREEN_HEIGHT / 9.375;
			playerButton = RoundedRectangle(SCREEN_WIDTH - buttonWidth * 1.5, SCREEN_HEIGHT - buttonHeight * 1.5, buttonWidth, buttonHeight, 10,
				darkGray, 2, sf::Color::Black);
			playerButton.setOutlineColor(sf::Color::Black);

			playerText.setFont(font);
			playerText.setCharacterSize(50.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
			playerText.setFillColor(sf::Color::Black);
			playerText.setStyle(sf::Text::Bold);
			playerText.setString("Back");
			sf::FloatRect textBounds = playerText.getLocalBounds();
			sf::FloatRect shapeBounds = playerButton.getLocalBounds();
			playerText.setPosition(shapeBounds.left + shapeBounds.width / 2 - textBounds.width / 2, shapeBounds.top);

			menuText[0].setFont(font);
			menuText[0].setCharacterSize(25.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
			menuText[0].setFillColor(sf::Color::Black);
			menuText[0].setStyle(sf::Text::Bold);
			menuText[0].setString("How to play! \n1. Pick your desired player count. \n2. Have each player select their Window! \nThe bottom right of each window displays \nit's difficulty from 3 "
				"(easiest) to 6 (hardest). \n3. Taking turns, each player will select a \n\"rolled\" dice and place it on their window. \nSee diagram at top right for placement \nrestrictions. The"
				" turn order is reversed after \nthe last person takes their turn (1,2,3,3,2,1). \n4. Once the first player finishes their second \nturn, if there are any remaining dice, they \nget"
				" placed on the track (this can be viewed \nat the bottom right of the player screen). \nIf there are more than one die remaining, \nonly one gets put on the track. This \nis "
				"considered the end of the round. \n5. Repeat steps 3 - 5 until 10 rounds have \nbeen completed. \n6. Scores will be calculated and the winner \nwill be revealed!");
			menuText[0].setPosition(10, 10);

			menuText[1].setFont(font);
			menuText[1].setCharacterSize(25.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
			menuText[1].setFillColor(sf::Color::Black);
			menuText[1].setStyle(sf::Text::Bold);
			menuText[1].setString("Dice Placement Restrictions: \n*First die: Each player's first die of \nthe game must be placed on an \nedge or corner space. \n*Every other die must be adjacent \nto"
				" previously placed die, touching \ndiagonally or orthogonally.\n*The die must match the color or \nvalue of the space. Grey spaces \nhave no restrictions \n*Dice may never be placed \n"
				"orthogonally adjecent to a die of \nthe same color or the same value. \n**NOTE: You can choose to not \ndraft a die by right clicking during \nyour turn.");
			menuText[1].setPosition(SCREEN_WIDTH / 1.75, 10);

			if (MouseInConvexShape(playerButton, &window)) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					inMenu = 0;
					Sleep(sleepTime);
				}
				playerButton.setOutlineColor(sf::Color::Yellow);
			}

			window.draw(menuText[0]);
			window.draw(menuText[1]);
			window.draw(playerButton);
			window.draw(playerText);
		}
		else if (playGame != playerCount - 1) { // let each player choose their window frame
			while (players.size() != playerCount) { // remove extra players if needed
				players.pop_back();
			}
			for (int i = 0; i < playerCount; i++) {
				if (players[i].boardIsEmpty()) { // for each player, let them choose their boards
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
					privColor.setFillColor(players.at(i).getPrivateObjective());
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
		else if (!(currentTurn == 10 && currentPlayer == 0)) { // play the game!
			if (dicePool.isCompletelyEmpty() && currentTurn == 1) {
				dicePool.roll(&diceBag, playerCount);
			}
			if (currentPlayerIsDone == 1 || currentPlayer == 0) {
				if (currentPlayer == 0) { // if round is over
					// reset draft pool, add remaining dice to round track
					roundTrack.setRoundTrack(dicePool, currentTurn);
					for (int r = 0; r < dicePool.size(); r++) {
						dicePool.removeDie(r);
					}
					dicePool.roll(&diceBag, playerCount);
					dicePoolShapes.clear();
					// set new player order
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
				// draw player board within boardOutline
				sf::ConvexShape boardOutline;
				float percentage = (float)SCREEN_WIDTH / (float)DEFAULT_SCREEN_WIDTH;
				float margin = 20.0 * percentage;
				float boardHeight = 400.0 * percentage;
				float boardWidth = 450.0 * percentage;
				boardOutline.setOrigin(boardWidth, 0);
				boardOutline = RoundedRectangle(SCREEN_WIDTH - margin - boardWidth, 0 + margin,
					boardWidth, boardHeight, 10, darkGray, 5, sf::Color::Black);
				window.draw(boardOutline);
				// draw the roundtrack
				sf::ConvexShape roundTrackFrame;
				float roundTrackHeight = 163.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				roundTrackFrame = RoundedRectangle(SCREEN_WIDTH - margin - boardWidth, 0 + boardHeight + 2.0 * margin,
					boardWidth, roundTrackHeight, 10, darkGray, 5, sf::Color::Black);
				window.draw(roundTrackFrame);
				for (int d = 0; d < 10; d++) {
					float roundTrackDieX, roundTrackDieY, roundTrackDieSize;
					roundTrackDieSize = roundTrackHeight / 2.6;
					sf::Color roundTrackDieOutlineColor = mediumGray;
					Die roundTrackDie;
					if (d < 5) {
						roundTrackDieX = roundTrackFrame.getGlobalBounds().left + margin * 4.5 * (d + 0.2);
						roundTrackDieY = roundTrackFrame.getGlobalBounds().top + margin * 0.9;
					}
					else {
						roundTrackDieX = roundTrackFrame.getGlobalBounds().left + margin * 4.5 * (d + 0.2 - 5);
						roundTrackDieY = roundTrackFrame.getGlobalBounds().top + roundTrackFrame.getGlobalBounds().height / 1.9;
					}
					if (roundTrack.getRoundTrack().size() > d) {
						roundTrackDie = roundTrack.getRoundTrack().at(d).at(0);
					}
					sf::RectangleShape roundTrackDieOutline;
					roundTrackDieOutline.setPosition(roundTrackDieX, roundTrackDieY);
					roundTrackDieOutline.setSize(sf::Vector2f(roundTrackDieSize, roundTrackDieSize));
					if (roundTrackDieOutline.getGlobalBounds().contains((sf::Vector2f)sf::Mouse::getPosition(window))) {
						if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) { //switch between selected die
						}
						// color on hover
						roundTrackDieOutlineColor = sf::Color::Yellow;
					}
					drawDie(roundTrackDie, roundTrackDieSize, roundTrackDieX, roundTrackDieY, roundTrackDieOutlineColor, Die(), &window);
				}
				std::vector <int> chosenDie = drawBoard(players[currentPlayer - 1].getBoard().getDice(), &window, boardOutline, dicePool.getDie(selectedDieIndex));
				if (chosenDie.size() == 2) { // if die has been clicked (place die in board)
					players[currentPlayer - 1].setDieInBoard(chosenDie[0], chosenDie[1], dicePool.getDie(selectedDieIndex));
					dicePool.removeDie(selectedDieIndex);
					dicePoolShapes.erase(dicePoolShapes.begin() + selectedDieIndex);
					currentPlayerIsDone = 1;
					selectedDieIndex = -1;
				}
				// draw token count and private objective in bottom of the board outline
				sf::RectangleShape privObj;
				privObj.setFillColor(players.at(currentPlayer - 1).getPrivateObjective());
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
				// draw who the current player is
				sf::Text currentPlayerText;
				currentPlayerText.setString("Player " + std::to_string(currentPlayer));
				currentPlayerText.setFont(font);
				currentPlayerText.setStyle(sf::Text::Bold);
				currentPlayerText.setFillColor(sf::Color::Black);
				currentPlayerText.setCharacterSize(40.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
				currentPlayerText.setPosition(SCREEN_WIDTH - margin * 1.4 - boardWidth / 1.5, 0 - margin * 2.8 + boardHeight);
				window.draw(currentPlayerText);
				sf::ConvexShape objectiveCards[3];
				float objectiveCardWidth = 290.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				float objectiveCardHeight = 181.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				objectiveCards[0] = RoundedRectangle(SCREEN_WIDTH - 2 * margin - boardWidth - objectiveCardWidth, 0 + margin, objectiveCardWidth, objectiveCardHeight, 10, darkGray, 5, sf::Color::Black);
				objectiveCards[1] = RoundedRectangle(SCREEN_WIDTH - 2 * margin - boardWidth - objectiveCardWidth, 0 + 2 * margin + objectiveCardHeight, objectiveCardWidth, objectiveCardHeight, 10, darkGray, 5, sf::Color::Black);
				objectiveCards[2] = RoundedRectangle(SCREEN_WIDTH - 2 * margin - boardWidth - objectiveCardWidth, 0 + 3 * margin + 2 * objectiveCardHeight, objectiveCardWidth, objectiveCardHeight, 10, darkGray, 5, sf::Color::Black);
				/*sf::ConvexShape toolCards[3];
				toolCards[0] = RoundedRectangle(SCREEN_WIDTH - 3 * margin - boardWidth - 2 * objectiveCardWidth, 0 + margin, objectiveCardWidth, objectiveCardHeight, 10, beige, 5, sf::Color::Black);
				toolCards[1] = RoundedRectangle(SCREEN_WIDTH - 3 * margin - boardWidth - 2 * objectiveCardWidth, 0 + 2 * margin + objectiveCardHeight, objectiveCardWidth, objectiveCardHeight, 10, beige, 5, sf::Color::Black);
				toolCards[2] = RoundedRectangle(SCREEN_WIDTH - 3 * margin - boardWidth - 2 * objectiveCardWidth, 0 + 3 * margin + 2 * objectiveCardHeight, objectiveCardWidth, objectiveCardHeight, 10, beige, 5, sf::Color::Black);*/
				//window.draw(toolCards[0]);
				//window.draw(toolCards[1]);
				//window.draw(toolCards[2]);
				sf::Text objectiveCardNames[3];
				sf::Text objectiveCardDescriptions[3];
				sf::Text objectiveCardPoints[3];
				for (int i = 0; i < 3; i++) { // draw the objective card text
					objectiveCardNames[i].setFont(font);
					objectiveCardNames[i].setStyle(sf::Text::Bold);
					objectiveCardNames[i].setLetterSpacing(0.48);
					objectiveCardNames[i].setFillColor(sf::Color::Black);
					objectiveCardDescriptions[i].setFont(font);
					objectiveCardDescriptions[i].setStyle(sf::Text::Bold);
					objectiveCardDescriptions[i].setLetterSpacing(0.5);
					objectiveCardDescriptions[i].setFillColor(sf::Color::Black);
					objectiveCardPoints[i].setFont(font);
					objectiveCardPoints[i].setStyle(sf::Text::Bold);
					objectiveCardPoints[i].setLetterSpacing(0.6);
					objectiveCardPoints[i].setFillColor(sf::Color::Black);
					objectiveCardNames[i].setString(selectedPublicObjectives[i].getName());
					int occur = 0;
					std::string description = selectedPublicObjectives[i].getDescription();
					for (int i = 0; i < description.length(); i++) {
						if (description.at(i) == ' ') {
							occur += 1;
						}
						if (description.at(10) == '&') {
							if (occur == 5) {
								occur = i;
								break;
							}
						}
						else if (occur == 3) {
							occur = i;
							break;
						}
					}
					description[occur] = '\n';
					objectiveCardDescriptions[i].setString(description);
					objectiveCardPoints[i].setString(selectedPublicObjectives[i].getPoints() + " per");
					objectiveCardNames[i].setPosition(objectiveCards[i].getGlobalBounds().left + margin / 1.8, objectiveCards[i].getGlobalBounds().top + margin / 2.0);
					objectiveCardNames[i].setCharacterSize(29.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
					objectiveCardDescriptions[i].setPosition(objectiveCards[i].getGlobalBounds().left + margin / 2.0, 
						objectiveCardNames[i].getGlobalBounds().top + objectiveCardNames[i].getGlobalBounds().height + margin / 1.15);
					objectiveCardDescriptions[i].setCharacterSize(25.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
					objectiveCardPoints[i].setPosition(objectiveCards[i].getGlobalBounds().left + margin / 2.0, 
						objectiveCardDescriptions[i].getGlobalBounds().top + objectiveCardDescriptions[i].getGlobalBounds().height + margin / 1.25);
					objectiveCardPoints[i].setCharacterSize(27.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
					window.draw(objectiveCards[i]);
					window.draw(objectiveCardNames[i]);
					window.draw(objectiveCardDescriptions[i]);
					window.draw(objectiveCardPoints[i]);
				}
				sf::ConvexShape draftPool;
				float draftPoolWidth = 300.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				float draftPoolHeight = 584.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
				draftPool = RoundedRectangle(0 + margin, 0 + margin, draftPoolWidth, draftPoolHeight, 10, darkGray, 5, sf::Color::Black);
				window.draw(draftPool);
				sf::RectangleShape draftPoolOutline; // used for setting the position of the dice, don't draw
				float diceMargin = margin * 1.75;
				draftPoolOutline.setPosition(0 + diceMargin, 0 + diceMargin);
				draftPoolOutline.setSize(sf::Vector2f(draftPool.getGlobalBounds().width - diceMargin * 1.25, draftPool.getGlobalBounds().height - diceMargin * 1.25));
				for (int d = 0; d < dicePool.size(); d++) { // draw each die in the draft pool
					float diceSize = 65.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
					float x, y;
					if (dicePoolShapes.size() != dicePool.size()) { // add dice to draft pool as needed
						std::uniform_int_distribution<int> diceDistX(0, (int)draftPoolOutline.getGlobalBounds().width - (int)diceSize);
						std::uniform_int_distribution<int> diceDistY(0, (int)draftPoolOutline.getGlobalBounds().height - (int)diceSize);
						x = diceDistX(mt) + draftPoolOutline.getGlobalBounds().left;
						y = diceDistY(mt) + draftPoolOutline.getGlobalBounds().top;
						sf::RectangleShape rectangle;
						rectangle.setPosition(x, y);
						rectangle.setSize(sf::Vector2f(diceSize, diceSize));
						bool intersects = false;
						for (int i = 0; i < dicePoolShapes.size(); i++) { // make sure the dice doesn't intersect
							sf::RectangleShape r = dicePoolShapes[i];
							sf::FloatRect checkGlobal = r.getGlobalBounds();
							checkGlobal.left *= ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
							checkGlobal.top *= ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
							checkGlobal.width *= ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
							checkGlobal.height *= ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
							checkGlobal.left -= 9.0;
							checkGlobal.top -= 9.0;
							checkGlobal.width += 18.0;
							checkGlobal.height += 18.0;
							if (rectangle.getGlobalBounds().intersects(checkGlobal)) {
								intersects = true;
								break;
							}
						}
						if (!intersects) {
							dicePoolShapes.push_back(drawDie(dicePool.getDie(d), diceSize, x / ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH), y / ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH), mediumGray, Die(), &window));
						}
						else { // redo if it intersects
							d--;
						}
					}
					else { // if the dice have already been added to the draft pool
						x = dicePoolShapes[d].getGlobalBounds().left * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
						y = dicePoolShapes[d].getGlobalBounds().top * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH);
						sf::RectangleShape currDice;
						currDice.setPosition(x, y);
						currDice.setSize(sf::Vector2f(diceSize, diceSize));
						if (currDice.getGlobalBounds().contains((sf::Vector2f)sf::Mouse::getPosition(window))) {
							if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) { // click draft pool die
								selectedDieIndex = d;
							}
							// color on hover
							drawDie(dicePool.getDie(d), diceSize, x, y, sf::Color::Yellow, Die(), &window);
						}
						else if (selectedDieIndex == d) {
							// color if selected
							drawDie(dicePool.getDie(d), diceSize, x, y, sf::Color::Green, Die(), &window);
						}
						else {
							drawDie(dicePool.getDie(d), diceSize, x, y, mediumGray, Die(), &window);
						}
						if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && selectedDieIndex != -1) { // right-button press deselects die
							selectedDieIndex = -1;
							Sleep(sleepTime);
						}
						else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) { // If right clicking with no die selected, turn is skipped
							currentPlayerIsDone = 1;
							Sleep(sleepTime);
						}
					}
				}
			}
		}
		else { // the game is over - tally up points and declare the winner
			// (\"id,score,isWinner\", ...) // int id, int score, bool isWinner (there can be two+ winners if there's a tie)
			*client.getOutBoundStream() << "(upd,)";
			float playerScoreMargin = SCREEN_WIDTH / 40.0;
			float playerScoreWidth = (SCREEN_WIDTH / 4.0) - playerScoreMargin;
			float playerScoreHeight = SCREEN_HEIGHT / 1.75;
			float totalWidth = playerCount * (playerScoreWidth + playerScoreMargin) - playerScoreMargin; // Calculate the total width of all the shapes
			float startX = (SCREEN_WIDTH - totalWidth) / 2.0; // Calculate the starting x-position to center the shapes
			//players.at(0).setFrame();
			for (int i = 0; i < playerCount; i++) {
				// add up from all three public objectives
				players.at(i).getTotalPoints(selectedPublicObjectives);
				// draw the outline of the scores
				sf::ConvexShape playerScoreOutline;
				float playerScoreX = startX + (playerScoreWidth + playerScoreMargin) * i;
				float playerScoreY = playerScoreMargin / 2.0;
				playerScoreOutline = RoundedRectangle(playerScoreX, playerScoreY, playerScoreWidth, playerScoreHeight, 10, players.at(i).getPrivateObjective(), 2, sf::Color::Black);
				window.draw(playerScoreOutline);
				// draw player name (#)
				sf::Text playerText;
				float playerTextX = playerScoreX + playerScoreWidth / 2.0;
				playerText.setFont(font);
				playerText.setFillColor(sf::Color::Black);
				playerText.setStyle(sf::Text::Bold);
				playerText.setString("Player " + std::to_string(i + 1));
				playerText.setCharacterSize(34.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
				playerText.setOrigin(playerText.getLocalBounds().width / 2, 0);
				playerText.setPosition(playerTextX, playerScoreY + playerScoreHeight / 30.0);
				window.draw(playerText);
				float textSize = 18.0;
				float pointsMarginDivider = 1.35;
				// draw public objective points gained
				std::vector<sf::Text> publicObjectiveNames;
				std::vector<sf::Text> publicObjectivePoints;
				for (int p = 0; p < selectedPublicObjectives.size(); p++) { // draw the objective card text
					publicObjectiveNames.push_back(sf::Text());
					float privateObjectiveY = 0.0;
					publicObjectiveNames[p].setFont(font);
					publicObjectiveNames[p].setStyle(sf::Text::Bold);
					publicObjectiveNames[p].setLetterSpacing(0.48);
					publicObjectiveNames[p].setFillColor(sf::Color::Black);
					publicObjectiveNames[p].setString(selectedPublicObjectives[p].getName() + ":");
					publicObjectiveNames[p].setCharacterSize(textSize * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
					publicObjectiveNames[p].setOrigin(publicObjectiveNames[p].getLocalBounds().left, 0);
					if (p == 0) {
						privateObjectiveY = playerText.getGlobalBounds().top + playerScoreMargin * 1.50;
						publicObjectiveNames[p].setPosition(playerScoreX + pointsMarginDivider * 10.00, privateObjectiveY);
					}
					else {
						privateObjectiveY = publicObjectiveNames[p - 1].getGlobalBounds().top + playerScoreMargin;
						publicObjectiveNames[p].setPosition(playerScoreX + pointsMarginDivider * 10.00, privateObjectiveY);
					}
					window.draw(publicObjectiveNames[p]);
					publicObjectivePoints.push_back(sf::Text());
					publicObjectivePoints[p].setFont(font);
					publicObjectivePoints[p].setStyle(sf::Text::Bold);
					publicObjectivePoints[p].setFillColor(sf::Color::Black);
					publicObjectivePoints[p].setString(std::to_string(players.at(i).getPublicObjectivePoints(p)));
					publicObjectivePoints[p].setCharacterSize(textSize * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
					publicObjectivePoints[p].setOrigin(publicObjectivePoints[p].getLocalBounds().width, 0);
					publicObjectivePoints[p].setPosition(playerScoreX + playerScoreWidth - pointsMarginDivider * 10.00, privateObjectiveY);
					window.draw(publicObjectivePoints[p]);
				}
				// draw private objective points gained
				sf::Text privateObjectiveText;
				sf::Text privateObjectivePoints;
				float privateObjectiveY = publicObjectivePoints[selectedPublicObjectives.size() - 1].getGlobalBounds().top + playerScoreMargin;
				privateObjectiveText.setFont(font);
				privateObjectiveText.setStyle(sf::Text::Bold);
				privateObjectiveText.setLetterSpacing(0.48);
				privateObjectiveText.setFillColor(sf::Color::Black);
				privateObjectiveText.setString("Private Objective Points:");
				privateObjectiveText.setCharacterSize(textSize * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
				privateObjectiveText.setOrigin(privateObjectiveText.getLocalBounds().left, 0);
				privateObjectiveText.setPosition(playerScoreX + pointsMarginDivider * 10.00, privateObjectiveY);
				window.draw(privateObjectiveText);
				privateObjectivePoints.setFont(font);
				privateObjectivePoints.setStyle(sf::Text::Bold);
				privateObjectivePoints.setFillColor(sf::Color::Black);
				privateObjectivePoints.setString(std::to_string(players.at(i).getPrivateObjectivePoints()));
				privateObjectivePoints.setCharacterSize(textSize * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
				privateObjectivePoints.setOrigin(privateObjectivePoints.getLocalBounds().width, 0);
				privateObjectivePoints.setPosition(playerScoreX + playerScoreWidth - pointsMarginDivider * 10.00, privateObjectiveY);
				window.draw(privateObjectivePoints);
				// draw open spaces points lost
				sf::Text openSpacesText, openSpacesPoints;
				float openSpacesY = privateObjectiveText.getGlobalBounds().top + playerScoreMargin;
				openSpacesText.setFont(font);
				openSpacesText.setStyle(sf::Text::Bold);
				openSpacesText.setLetterSpacing(0.48);
				openSpacesText.setFillColor(sf::Color::Black);
				openSpacesText.setString("Open Spaces:");
				openSpacesText.setCharacterSize(textSize * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
				openSpacesText.setOrigin(openSpacesText.getLocalBounds().left, 0);
				openSpacesText.setPosition(playerScoreX + pointsMarginDivider * 10.00, openSpacesY);
				window.draw(openSpacesText);
				openSpacesPoints.setFont(font);
				openSpacesPoints.setStyle(sf::Text::Bold);
				openSpacesPoints.setFillColor(sf::Color::Black);
				openSpacesPoints.setString(std::to_string(0 - players.at(i).getBoard().countEmpty()));
				openSpacesPoints.setCharacterSize(textSize * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
				openSpacesPoints.setOrigin(openSpacesPoints.getLocalBounds().width, 0);
				openSpacesPoints.setPosition(playerScoreX + playerScoreWidth - pointsMarginDivider * 10.00, openSpacesY);
				window.draw(openSpacesPoints);
				// draw total points
				sf::Text totalPointsText, totalPointsPoints;
				float totalPointsY = openSpacesText.getGlobalBounds().top + playerScoreMargin;
				totalPointsText.setFont(font);
				totalPointsText.setStyle(sf::Text::Bold);
				totalPointsText.setLetterSpacing(0.4);
				totalPointsText.setFillColor(sf::Color::Black);
				totalPointsText.setString("TOTAL POINTS:");
				totalPointsText.setCharacterSize(textSize * 1.4 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
				totalPointsText.setOrigin(totalPointsText.getLocalBounds().left, 0);
				totalPointsText.setPosition(playerScoreX + pointsMarginDivider * 10.00, totalPointsY);
				window.draw(totalPointsText);
				totalPointsPoints.setFont(font);
				totalPointsPoints.setStyle(sf::Text::Bold);
				totalPointsPoints.setFillColor(sf::Color::Black);
				totalPointsPoints.setString(std::to_string(players.at(i).getTotalPoints()));
				totalPointsPoints.setCharacterSize(textSize * 1.4 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
				totalPointsPoints.setOrigin(totalPointsPoints.getLocalBounds().width, 0);
				totalPointsPoints.setPosition(playerScoreX + playerScoreWidth - pointsMarginDivider * 10.00, totalPointsY);
				window.draw(totalPointsPoints);
			}
			std::vector<int> winners = { 1 };
			int max_points = players.at(0).getTotalPoints(); // assume first element is the maximum
			int count_max = 1; // count the number of times the maximum appears
			for (int i = 0; i < playerCount; i++) {
				if (players.at(i).getTotalPoints() > max_points) {
					winners.clear();
					max_points = players.at(i).getTotalPoints();
					count_max = 1;
					winners.push_back(i + 1);
				}
				else if (players.at(i).getTotalPoints() == max_points) {
					if (i != 0) {
						count_max++;
						winners.push_back(i + 1);
					}
				}
			}
			std::string winnerString;
			if (count_max > 1) { // Ties are broken by Private Objective points
				std::vector<int> privateObjectivePoints;
				for (int p = 0; p < winners.size(); p++) {
					privateObjectivePoints.push_back(players.at(winners.at(p) - 1).getPrivateObjectivePoints());
				}
				std::vector<int> realWinners = { };
				max_points = privateObjectivePoints.at(0); 
				count_max = 1;
				for (int i = 0; i < winners.size(); i++) {
					if (privateObjectivePoints.at(i) > max_points) {
						realWinners.clear();
						max_points = privateObjectivePoints.at(i);
						count_max = 1;
						realWinners.push_back(i + 1);
					}
					else if (privateObjectivePoints.at(i) == max_points) {
						count_max++;
						realWinners.push_back(i + 1);
					}
				}
				if (realWinners.size() > 1) {
					winnerString = "Players ";
					for (int o = 0; o < realWinners.size(); o++) {
						winnerString.append(std::to_string(realWinners.at(o)) + " ");
						if (o != realWinners.size() - 1) {
							winnerString.append("and ");
						}
					}
					winnerString.append("have TIED!");
					//std::cout << "There is a tie between the winners!" << std::endl;
				}
				else {
					winnerString = "Player " + std::to_string(realWinners.at(0)) + " WINS via a TIEBREAKER!";
				}
			}
			else {
				winnerString = "Player " + std::to_string(winners.at(0)) + " WINS!";
				//std::cout << "There is no tie between the winners." << std::endl;
			}

			sf::Text winnerText;
			winnerText.setFont(font);
			winnerText.setFillColor(sf::Color::Black);
			winnerText.setStyle(sf::Text::Bold);
			winnerText.setCharacterSize(60.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
			winnerText.setString(winnerString);
			winnerText.setOrigin(winnerText.getLocalBounds().width / 2, 0);
			winnerText.setPosition(SCREEN_WIDTH / 2.0, SCREEN_HEIGHT - margin * 2.0);
			window.draw(winnerText);
			sf::Text playAgain;
			playAgain.setFont(font);
			playAgain.setFillColor(sf::Color::Black);
			playAgain.setStyle(sf::Text::Bold);
			playAgain.setCharacterSize(20.0 * ((double)SCREEN_WIDTH / (double)DEFAULT_SCREEN_WIDTH));
			playAgain.setString("Right-click to exit the game");
			playAgain.setOrigin(playAgain.getLocalBounds().width / 2, 0);
			playAgain.setPosition(SCREEN_WIDTH / 2.0, SCREEN_HEIGHT - margin / 2.5);
			window.draw(playAgain);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) { // right-click to exit the game
				playerCount = 0;
				currentPlayer = 1;
				currentTurn = 1;
				playGame = 0;
				currentPlayerIsDone = 0;
				Sleep(sleepTime);
			}
		}
        window.display();
    }

	// terminate server
	client.killserver();
	tcpClient.join();

    return 0;
}