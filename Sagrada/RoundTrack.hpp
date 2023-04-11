#pragma once
#include <vector>
#include "Die.hpp"
#include "DicePool.hpp"

class RoundTrack
{
public:
	std::vector<std::vector<Die>> getRoundTrack();
	void setRoundTrack(std::vector<std::vector<Die>> roundTrack); // set the whole roundtrack
	void setRoundTrack(std::vector<Die> diceStack, int index); // set one index of the roundtrack
	void setRoundTrack(DicePool diceStack, int index); // set one index of the roundtrack
private:
	std::vector<std::vector<Die>> roundTrack;
};

