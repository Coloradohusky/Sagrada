#include <SFML/Graphics.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <set>
#include "Die.hpp"
#include "PatternCard.hpp"

// 1920x1080 = 16:9
// 240:127 = fullscreen on 16:9
const double ratio = 9.0 / 16.0;
const int DEFAULT_SCREEN_WIDTH = 1120;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
const int sleepTime = 200;
const double margin = 80.0;
const sf::Color darkGray(145, 150, 150, 255);
const sf::Color gray(146, 153, 152, 255);
const sf::Color beige(253, 217, 150, 255);

sf::ConvexShape RoundedRectangle(float x, float y, float rectWidth, float rectHeight, float radius, const sf::Color& shapeColor, float outline = 0.f, const sf::Color& outlineCol = sf::Color(0, 0, 0));

bool MouseInConvexShape(sf::ConvexShape shape, sf::Window* window);

std::vector<std::vector<bool>> determinePlacementSpots(std::vector<std::vector<Die>> board, Die selectedDie, std::string restrictions);

void fillFrameArray(boost::property_tree::ptree& pt, std::vector<PatternCard>* frameData, int index, int subIndex, int& subSubIndex);

int drawFrame(std::string frameName, std::vector<std::vector<Die>> frameBoard, int frameTokens, sf::RenderWindow* window, int position);

int drawBoard(std::vector<std::vector<Die>> frameBoard, sf::RenderWindow* window, sf::ConvexShape border, Die selectedDie);
