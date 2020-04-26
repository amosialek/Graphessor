#include "RivaraUtils.hpp"
#include "gtest/gtest.h"
#include "Pixel.hpp"

using namespace Rivara;

TEST(RUTest, NLPointToPointPitagoras)
{
    Pixel p1, p2;
    p1.x = 1;
    p1.y = 4;
    p2.x = 5;
    p2.y = 1;
    double actual = NL(p1,p2);
    double expected = 5.0;
    ASSERT_DOUBLE_EQ(expected, actual);
}

TEST(RUTest, NLCoordsPitagoras)
{
    int x1 = 1;
    int y1 = 4;
    int x2 = 5;
    int y2 = 1;
    double actual = NL(x1,y1,x2,y2);
    double expected = 5.0;
    ASSERT_DOUBLE_EQ(expected, actual);
}