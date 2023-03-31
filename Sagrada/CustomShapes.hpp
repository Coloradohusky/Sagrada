#include <SFML/Graphics.hpp>

sf::ConvexShape RoundedRectangle(float x, float y, float rectWidth, float rectHeight, float radius, const sf::Color& shapeColor, float outline = 0.f, const sf::Color& outlineCol = sf::Color(0, 0, 0));

bool MouseInConvexShape(sf::ConvexShape shape, sf::Window* window);