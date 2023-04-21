#include "pch.h"

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(PlayerBoardTest, PlayerBoardScoreTest) {
    PlayerBoard board;
    // Example board taken from the Sagrada rulebook
    std::vector<std::vector<Die>> testBoard{
        {Die(),Die("Green", 4),Die("Red", 2),Die("Yellow", 5),Die("Purple", 6)},
        {Die("Red", 3),Die("Blue", 1),Die(2),Die("Green", 1),Die("Yellow", 2)},
        {Die("Purple", 5),Die("Green", 6),Die("Red"),Die("Purple", 6),Die("Red", 3)},
        {Die("Blue", 4),Die("Yellow", 3),Die("Green", 4),Die("Blue", 2),Die("Green", 4)}
    };
    board.setFrame(testBoard);
    std::vector<PublicObjective> selectedPublicObjectives{
        PublicObjective(), PublicObjective(), PublicObjective()
    };
    selectedPublicObjectives.at(0).setName("Column Color Variety");
    selectedPublicObjectives.at(0).setPoints("5");
    selectedPublicObjectives.at(1).setName("Light Shades");
    selectedPublicObjectives.at(1).setPoints("2");
    selectedPublicObjectives.at(2).setName("Color Variety");
    selectedPublicObjectives.at(2).setPoints("4");
    ASSERT_EQ(scoreBoard(board, selectedPublicObjectives, "Blue"), 40);
}