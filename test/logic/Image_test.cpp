#include "gtest/gtest.h"
#include "Image.hpp"
#include "Productions.hpp"
using namespace std;

TEST(ImageComparisonTest,test1)
{
    auto image = new Image("./test_files/face.bmp"); 
    auto image2 = new Image("test_files/face_test.bmp"); 
    int a = image->CompareWith(*image2);;
    EXPECT_EQ(363,a);
}


TEST(ImageComparisonTest, SquaredErrorOfInterpolation)
{
    uint8_t*** test_img;
    test_img = new uint8_t**[5];
    for(int i=0;i<5;i++)
    {
        test_img[i] = new uint8_t*[5];
        for(int j=0;j<5;j++)
            test_img[i][j] = new uint8_t[1];
    }

    uint8_t array [5][5][1] =
    {
    {{1}, {1}, {1}, {1}, {1}},
    {{1}, {1}, {1}, {2}, {1}},
    {{1}, {1}, {2}, {1}, {2}},
    {{1}, {1}, {1}, {2}, {1}},
    {{1}, {2}, {2}, {1}, {2}}
    };
    for(int i=0;i<5;i++)
        for(int j=0;j<5;j++)
            test_img[i][j][0]=array[i][j][0];

    auto image = new Image(test_img,5,5,1); 
    
    double a = image->SquaredErrorOfInterpolation(0,1,0,1,0);
    EXPECT_DOUBLE_EQ(0,a);
    a = image->SquaredErrorOfInterpolation(2,4,0,1,0);
    EXPECT_DOUBLE_EQ(0.25,a);
    a = image->SquaredErrorOfInterpolation(0,1,2,4,0);
    EXPECT_DOUBLE_EQ(1,a);
    a = image->SquaredErrorOfInterpolation(2,4,2,4,0);
    EXPECT_DOUBLE_EQ(4,a);
}

TEST(ImageComparisonTest, CompareWithInterpolation)
{
    uint8_t*** test_img;
    test_img = new uint8_t**[5];
    for(int i=0;i<5;i++)
    {
        test_img[i] = new uint8_t*[5];
        for(int j=0;j<5;j++)
            test_img[i][j] = new uint8_t[1];
    }

    uint8_t array [5][5][1] =
    {
    {{0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {100}, {0}},
    {{0}, {0}, {100}, {0}, {100}},
    {{0}, {0}, {0}, {100}, {0}},
    {{0}, {100}, {100}, {0}, {100}}
    };
    for(int i=0;i<5;i++)
        for(int j=0;j<5;j++)
            test_img[i][j][0]=array[i][j][0];

    auto image = new Image(test_img,5,5,1); 
    
    double a = image->CompareWithInterpolation(0,1,0,1,0);
    EXPECT_DOUBLE_EQ(0,a);
    a = image->CompareWithInterpolation(2,4,0,1,0);
    EXPECT_DOUBLE_EQ(2500.0/(255*255*6),a);
    a = image->CompareWithInterpolation(0,1,2,4,0);
    EXPECT_DOUBLE_EQ(10000.0/(255*255*6),a);
    a = image->CompareWithInterpolation(2,4,2,4,0);
    EXPECT_DOUBLE_EQ(40000.0/(255*255*9),a);
    
}

TEST(ImageMagnifierComparisonTest, CompareWithInterpolation)
{
    uint8_t*** test_img;
    test_img = new uint8_t**[5];
    for(int i=0;i<5;i++)
    {
        test_img[i] = new uint8_t*[5];
        for(int j=0;j<5;j++)
            test_img[i][j] = new uint8_t[1];
    }

    uint8_t array [5][5][1] =
    {
    {{0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {100}, {0}},
    {{0}, {0}, {100}, {0}, {100}},
    {{0}, {0}, {0}, {100}, {0}},
    {{0}, {100}, {100}, {0}, {100}}
    };
    for(int i=0;i<5;i++)
        for(int j=0;j<5;j++)
            test_img[i][j][0]=array[i][j][0];

    auto image = new ImageMagnifier(test_img,5,5,1); 
    
    double a = image->CompareWithInterpolation(0,4,0,4,0);
    EXPECT_DOUBLE_EQ(0,a);
    a = image->CompareWithInterpolation(2,4,0,4,0);
    EXPECT_DOUBLE_EQ(2500.0/(255*255*6),a);
    a = image->CompareWithInterpolation(0,4,8,16,0);
    EXPECT_DOUBLE_EQ(10000.0/(255*255*6),a);
    a = image->CompareWithInterpolation(8,16,8,16,0);
    EXPECT_DOUBLE_EQ(40000.0/(255*255*9),a);
    
}