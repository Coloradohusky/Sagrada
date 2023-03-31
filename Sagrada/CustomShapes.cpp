#include "CustomShapes.hpp"
#include <iostream>

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