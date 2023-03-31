#include "RoundTrack.hpp"

std::vector<std::vector<Die>> RoundTrack::getRoundTrack() {
    return roundTrack;
}

void RoundTrack::setRoundTrack(std::vector<std::vector<Die>> newRoundTrack) {
    roundTrack = newRoundTrack;
}

void RoundTrack::setRoundTrack(std::vector<Die> diceStack, int index) {
    roundTrack[index] = diceStack;
}
