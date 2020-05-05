#include "gtest/gtest.h"
#include "Image_test.cpp"
#include "Production_test.cpp"
#include "RivaraProduction1test.cpp"
#include "RivaraProduction2test.cpp"
#include "RivaraProduction3test.cpp"
#include "RivaraProduction4test.cpp"
#include "RivaraProduction5test.cpp"
#include "RivaraProduction6test.cpp"
using namespace std;
int main()
{
    testing::InitGoogleTest();
    auto a = RUN_ALL_TESTS();
    return a;
}

