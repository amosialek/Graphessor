#include "gtest/gtest.h"
#include "PbiHelper.hpp"

using namespace std;

TEST(PbiHelperTests, GetSquaredIntegral)
{
    auto fun = GetXSquareIntegral(1,2);
    ASSERT_EQ(fun(1,1), 1/3.0-3/2.0+2);
    ASSERT_EQ(fun(2,1), 8/3.0-12/2.0+4);
}

TEST(PbiHelperTests, GetX4Integral)
{
    auto fun = GetX4Integral(1,2);
    ASSERT_EQ(fun(1,1), 1/5.0 - 3/2.0 + 13/3.0 - 6 + 4);
    ASSERT_EQ(fun(2,1), 32/5.0 - 24 + 13*8/3.0 - 24 + 8);
}

TEST(PbiHelperTests, GetX4Integral2)
{
    auto fun = GetX4Integral(0,0);
    ASSERT_EQ(fun(1,1), 1/5.0 );
    ASSERT_EQ(fun(2,1), 32/5.0);
}

TEST(PbiHelperTests, GetX4Integral3)
{
    auto fun = GetX4Integral(0,1);
    ASSERT_EQ(fun(1,1), 1/5.0 - 1/2.0 + 1/3.0);
    ASSERT_EQ(fun(2,1), 32/5.0 - 8 + 8/3.0);
}

TEST(PbiHelperTests, GetSquareInterpolationOfEdge)
{
    Array2D a = Array2D(11,1);
    for(int i=0;i<11;i++)
        a[i][0] = i*(10.0-i)/2.0;
    auto coefficient = GetSquareInterpolationOfEdge(a,0,10,0);
    ASSERT_TRUE(abs(coefficient+0.5)<0.1);
}

TEST(PbiHelperTests, GetSquareInterpolationOfYEdge)
{
    Array2D a = Array2D(1,11);
    for(int i=0;i<11;i++)
        a[0][i] = i*(10.0-i)/2.0;
    auto coefficient = GetSquareInterpolationOfYEdge(a,0,0,10);
    ASSERT_TRUE(abs(coefficient+0.5)<0.1);
}

TEST(PbiHelperTests, GetSquareInterpolationOfEdge2)
{
    Array2D a = Array2D(2000,2000);
    a.FillWith(0);
    a[1787][1999]=-0.5;
    double y = GetSquareInterpolationOfEdge(a, 1786, 1788, 1999);
    ASSERT_FALSE(std::isnan(y));
    ASSERT_NE(y,0);
}

TEST(PbiHelperTests, GetSquareInterpolationOfYEdge2)
{
    Array2D a = Array2D(2000,2000);
    a.FillWith(0);
    a[1999][1787]=-0.5;
    double y = GetSquareInterpolationOfYEdge(a, 1999, 1786, 1788);
    ASSERT_FALSE(std::isnan(y));
    ASSERT_NE(y,0);
}

TEST(PbiHelperTests, GetSquareInterpolationOfRectangle)
{
    Array2D a = Array2D(1001,1001);
    for(int x=0;x<1001;x++)
        for(int y=0;y<1001;y++)
            a[x][y] = x*(1000.0-x)*y*(1000.0-y)/2.0;
    auto coefficient = GetSquareInterpolationOfRectangle(a, 0, 1000, 0, 1000);
     ASSERT_TRUE(abs(coefficient+0.5)<0.1);
}

TEST(PbiHelperTests, GetSquareInterpolationOfRectangle2)
{
    Array2D a = Array2D(5001,5001);
    for(int x=4000;x<5001;x++)
        for(int y=4000;y<5001;y++)
            a[x][y] = (x-4000)*(5000.0-x)*(y-4000)*(5000.0-y)/2.0;
    auto coefficient = GetSquareInterpolationOfRectangle(a, 4000, 5000, 4000, 5000);
     ASSERT_TRUE(abs(coefficient+0.5)<0.1);
}