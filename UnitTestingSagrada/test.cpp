#include "pch.h"

//note if you cant use classes like usual make sure to include them in the PCH file or precompiled header file

TEST(TestCaseName, TestName) {

  //Expect_EQ stands for expect two things are equal.
  //expect is non fatal meaning if this is wrong the test continues through but its still a fail 
  EXPECT_EQ(1, 1);

  //assert eq stands for assert two things are equal
  //failing this is fatal test stops immediately and fails
  ASSERT_EQ(true, true);
}


