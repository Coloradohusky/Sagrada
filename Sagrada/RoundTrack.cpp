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

void RoundTrack::setRoundTrack(DicePool diceStack, int index) {
    roundTrack.push_back(std::vector<Die>());
    for (int d = 0; d < diceStack.size(); d++) {
        Die newDie = diceStack.getDie(d);
        roundTrack[index - 1].push_back(newDie);
    }
}
