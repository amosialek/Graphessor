#include "gtest/gtest.h"
#include "Image_test.cpp"
using namespace std;
int main()
{
    testing::InitGoogleTest();
    auto a = RUN_ALL_TESTS();
    return a;
}

