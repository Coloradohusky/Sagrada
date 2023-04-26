#include <SFML/Graphics.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <set>
#include "Die.hpp"
#include "PatternCard.hpp"
#include "PublicObjective.hpp"

#pragma warning(disable: 4244)
#pragma warning(disable: 4267)
#pragma warning(disable: 4305)
#pragma warning(disable: 26451)

// 1920x1080 = 16:9
// 240:127 = fullscreen on 16:9
const double ratio = 127.0 / 240.0;
const int DEFAULT_SCREEN_WIDTH = 1120;
const int DEFAULT_SCREEN_HEIGHT = 630;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
const int sleepTime = 200;
const double margin = 80.0;
const sf::Color darkGray(145, 150, 150, 255);
const sf::Color gray(146, 153, 152, 255);
const sf::Color beige(253, 217, 150, 255);
const sf::Color blue(45, 187, 200, 255);
const sf::Color red(220, 35, 39, 255);
const sf::Color green(3, 171, 108, 255);
const sf::Color purple(165, 65, 152, 255);
const sf::Color yellow(243, 222, 12, 255);
const sf::Color lightGray(202, 206, 205, 255);
const sf::Color mediumGray(99, 105, 104, 255);

sf::ConvexShape RoundedRectangle(double x, double y, double rectWidth, double rectHeight, double radius, const sf::Color& shapeColor, double outline = 0.f, const sf::Color& outlineCol = sf::Color(0, 0, 0));

bool MouseInConvexShape(sf::ConvexShape shape, sf::Window* window);

std::vector<std::vector<bool>> determinePlacementSpots(std::vector<std::vector<Die>> board, Die selectedDie, std::string restrictions);

void fillFrameArray(boost::property_tree::ptree& pt, std::vector<PatternCard>* frameData, int index, int subIndex, int& subSubIndex);

int drawFrame(std::string frameName, std::vector<std::vector<Die>> frameBoard, int frameTokens, sf::RenderWindow* window, int position);

std::vector<int> drawBoard(std::vector<std::vector<Die>> frameBoard, sf::RenderWindow* window, sf::ConvexShape border, Die selectedDie);

sf::RectangleShape drawDie(Die currentDie, double size, double x, double y, sf::Color isClickableOutlineColor, Die selectedDie, sf::RenderWindow* window);