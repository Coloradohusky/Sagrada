#include "pch.h"

// example test
TEST(ExampleTest, ExampleTestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(PlayerBoardTest, TestRulebookExample) {
    Player player(1);
    player.setPrivateObjective("Purple");
    // Example board taken from the Sagrada rulebook
    std::vector<std::vector<Die>> testBoard{
        {Die(),Die("Green", 4),Die("Red", 2),Die("Yellow", 5),Die("Purple", 6)},
        {Die("Red", 3),Die("Blue", 1),Die(2),Die("Green", 1),Die("Yellow", 2)},
        {Die("Purple", 5),Die("Green", 6),Die("Red"),Die("Purple", 6),Die("Red", 3)},
        {Die("Blue", 4),Die("Yellow", 3),Die("Green", 4),Die("Blue", 2),Die("Green", 4)}
    };
    player.setFrame(testBoard);
    std::vector<PublicObjective> selectedPublicObjectives{
        PublicObjective(), PublicObjective(), PublicObjective()
    };
    selectedPublicObjectives.at(0).setName("Column Color Variety");
    selectedPublicObjectives.at(0).setPoints("5");
    selectedPublicObjectives.at(1).setName("Light Shades");
    selectedPublicObjectives.at(1).setPoints("2");
    selectedPublicObjectives.at(2).setName("Color Variety");
    selectedPublicObjectives.at(2).setPoints("4");
    ASSERT_EQ(player.getTotalPoints(selectedPublicObjectives), 40);
}

TEST(PlayerBoardTest, TestColorDiagonals) {
    Player player(1);
    player.setPrivateObjective("NONE");
    // Example board taken from the Sagrada rulebook
    std::vector<std::vector<Die>> testBoard{
        {Die("Green", 3),Die("Purple", 1),Die("Red", 3),Die("Green", 6),Die("Yellow", 2)},
        {Die("Blue", 5),Die("Green", 3),Die("Blue", 6),Die("Purple", 5),Die("Green", 3)},
        {Die("Yellow", 3),Die("Blue", 6),Die("Green", 1),Die("Red", 2),Die("Blue", 4)},
        {Die("Purple", 2),Die("Yellow", 5),Die("Red", 6),Die("Green", 4),Die("Purple", 1)}
    };
    player.setFrame(testBoard);
    std::vector<PublicObjective> selectedPublicObjectives{
        PublicObjective()
    };
    selectedPublicObjectives.at(0).setName("Color Diagonals");
    selectedPublicObjectives.at(0).setPoints("#");
    ASSERT_EQ(player.getTotalPoints(selectedPublicObjectives), 13);
}

TEST(PlayerBoardTest, TestCounts) {
    PlayerBoard board;
    //player.setPrivateObjective("NONE");
    // Example board taken from the Sagrada rulebook
    std::vector<std::vector<Die>> testBoard{
        {Die("Green", 3),Die("Purple", 1),Die("Red", 3),Die("Green", 6),Die("Yellow", 2)},
        {Die("Blue", 5),Die("Green", 3),Die("Blue", 6),Die("Purple", 5),Die("Green", 3)},
        {Die("Yellow", 3),Die("Blue", 6),Die("Green", 1),Die("Red", 2),Die("Blue", 4)},
        {Die("Purple", 2),Die("Yellow", 5),Die("Red", 6),Die("Green", 4),Die("Purple", 1)}
    };
    board.setFrame(testBoard);
    ASSERT_EQ(board.isEmpty(), false);
    ASSERT_EQ(board.isCompletelyEmpty(), false);
    ASSERT_EQ(board.countEmpty(), 0);
    ASSERT_EQ(board.countValue(4), 2);
    ASSERT_EQ(board.countColor("Green"), 6);
    ASSERT_EQ(board.countColorSum("Green"), 20);
    ASSERT_EQ(board.getDice().size(), testBoard.size());
}

TEST(PlayerTest, TestGetters) {
    Player player(1);
    player.setPrivateObjective("Green");
    player.setTokens(5);
    ASSERT_EQ(player.getPrivateObjective(), "Green");
    ASSERT_EQ(player.getPrivateObjectiveFillColor(), green);
    ASSERT_EQ(player.getTokens(), 5);
}

TEST(PublicObjectiveTest, TestGetters) {
    PublicObjective p;
    p.setDescription("This is a public objective.");
    p.setName("Public Objective");
    p.setPoints("#");
    ASSERT_EQ(p.getDescription(), "This is a public objective.");
    ASSERT_EQ(p.getName(), "Public Objective");
    ASSERT_EQ(p.getPoints(), "#");
}