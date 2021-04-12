#include "gtest/gtest.h"
#include "Array2D.hpp"
#include "Productions.hpp"
using namespace std;

TEST(Array2DTest, SquaredError)
{
    uint8_t*** test_img;
    test_img = new uint8_t**[5];
    for(int i=0;i<5;i++)
    {
        test_img[i] = new uint8_t*[5];
        for(int j=0;j<5;j++)
            test_img[i][j] = new uint8_t[1];
    }

    std::vector<std::vector<double>> array =
    {
    {{1}, {1}, {1}, {1}, {1}},
    {{1}, {1}, {1}, {2}, {1}},
    {{1}, {1}, {2}, {1}, {2}},
    {{1}, {1}, {1}, {2}, {1}},
    {{1}, {2}, {2}, {1}, {2}}
    };

    auto image = new Array2D(array); 
    auto interpolation = new Array2D(array); 
    interpolation->FillWith(0);
    interpolation->TrivialBilinearInterpolation(*image, 0,1,0,1);
    double a = image->SquaredError(*interpolation, 0,1,0,1);
    EXPECT_DOUBLE_EQ(0,a);
    interpolation->FillWith(0);
    interpolation->TrivialBilinearInterpolation(*image, 2,4,0,1);
    a = image->SquaredError(*interpolation, 2,4,0,1);
    EXPECT_DOUBLE_EQ(0.25,a);
    interpolation->FillWith(0);
    interpolation->TrivialBilinearInterpolation(*image, 0,1,2,4);
    a = image->SquaredError(*interpolation, 0,1,2,4);
    EXPECT_DOUBLE_EQ(1,a);
    interpolation->FillWith(0);
    interpolation->TrivialBilinearInterpolation(*image, 2,4,2,4);
    a = image->SquaredError(*interpolation, 2,4,2,4);
    EXPECT_DOUBLE_EQ(4,a);
}

TEST(Array2DTest, CompareWithInterpolation)
{
    uint8_t*** test_img;
    test_img = new uint8_t**[5];
    for(int i=0;i<5;i++)
    {
        test_img[i] = new uint8_t*[5];
        for(int j=0;j<5;j++)
            test_img[i][j] = new uint8_t[1];
    }

    std::vector<std::vector<double>> array  =
    {
    {{0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {100}, {0}},
    {{0}, {0}, {100}, {0}, {100}},
    {{0}, {0}, {0}, {100}, {0}},
    {{0}, {100}, {100}, {0}, {100}}
    };

    auto image = new Array2D(array); 
    auto interpolation = new Array2D(array); 
    
    interpolation->FillWith(0);
    interpolation->TrivialBilinearInterpolation(*image,0,1,0,1);
    double a = image->CompareWith(*interpolation, 0,1,0,1);
    EXPECT_DOUBLE_EQ(0,a);
    interpolation->FillWith(0);
    interpolation->TrivialBilinearInterpolation(*image,2,4,0,1);
    a = image->CompareWith(*interpolation, 2,4,0,1);
    EXPECT_DOUBLE_EQ(2500.0/(255*255*6),a);
    interpolation->FillWith(0);
    interpolation->TrivialBilinearInterpolation(*image,0,1,2,4);
    a = image->CompareWith(*interpolation, 0,1,2,4);
    EXPECT_DOUBLE_EQ(10000.0/(255*255*6),a);
    interpolation->FillWith(0);
    interpolation->TrivialBilinearInterpolation(*image,2,4,2,4);
    a = image->CompareWith(*interpolation, 2,4,2,4);
    EXPECT_DOUBLE_EQ(40000.0/(255*255*9),a);
    
}

// TEST(ImageMagnifierComparisonTest, CompareWithInterpolation)
// {
//     uint8_t*** test_img;
//     test_img = new uint8_t**[5];
//     for(int i=0;i<5;i++)
//     {
//         test_img[i] = new uint8_t*[5];
//         for(int j=0;j<5;j++)
//             test_img[i][j] = new uint8_t[1];
//     }

//     uint8_t array [5][5][1] =
//     {
//     {{0}, {0}, {0}, {0}, {0}},
//     {{0}, {0}, {0}, {100}, {0}},
//     {{0}, {0}, {100}, {0}, {100}},
//     {{0}, {0}, {0}, {100}, {0}},
//     {{0}, {100}, {100}, {0}, {100}}
//     };
//     for(int i=0;i<5;i++)
//         for(int j=0;j<5;j++)
//             test_img[i][j][0]=array[i][j][0];

//     auto image = new ImageMagnifier(test_img,5,5,1); 
    
//     double a = image->CompareWithInterpolation(0,4,0,4,0);
//     EXPECT_DOUBLE_EQ(0,a);
//     a = image->CompareWithInterpolation(8,16,0,4,0);
//     EXPECT_DOUBLE_EQ(2500.0/(255*255*6),a);
//     a = image->CompareWithInterpolation(0,4,8,16,0);
//     EXPECT_DOUBLE_EQ(10000.0/(255*255*6),a);
//     a = image->CompareWithInterpolation(8,16,8,16,0);
//     EXPECT_DOUBLE_EQ(40000.0/(255*255*9),a);
    
// }

// TEST(ImageMagnifierComparisonTest, CompareWithInterpolation2)
// {
//     uint8_t*** test_img;
//     test_img = new uint8_t**[5];
//     for(int i=0;i<5;i++)
//     {
//         test_img[i] = new uint8_t*[5];
//         for(int j=0;j<5;j++)
//             test_img[i][j] = new uint8_t[3];
//     }

//     uint8_t*** test_img2;
//     test_img2 = new uint8_t**[5];
//     for(int i=0;i<5;i++)
//     {
//         test_img2[i] = new uint8_t*[5];
//         for(int j=0;j<5;j++)
//             test_img2[i][j] = new uint8_t[3];
//     }

//     uint8_t array [5][5][3] =
//     {
//     {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
//     {{0,0,0}, {0,0,0}, {0,0,0}, {100,0,0}, {0,0,0}},
//     {{0,0,0}, {0,0,0}, {0,100,0}, {0,0,0}, {100,0,0}},
//     {{0,0,0}, {0,0,0}, {0,0,0}, {100,0,0}, {0,0,0}},
//     {{0,0,0}, {100,0,0}, {100,0,0}, {0,0,0}, {100,0,0}}
//     };
//     for(int i=0;i<5;i++)
//         for(int j=0;j<5;j++)
//             for(int k=0;k<3;k++)
//                 test_img[i][j][k]=array[i][j][k];

//     uint8_t array2 [5][5][3] =
//     {
//     {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
//     {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
//     {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
//     {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
//     {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}}
//     };
//     for(int i=0;i<5;i++)
//         for(int j=0;j<5;j++)
//             for(int k=0;k<5;k++)
//                 test_img2[i][j][k]=array2[i][j][k];

//     auto image = new ImageMagnifier(test_img,5,5,3);    
//     auto image2 = new Image(test_img2,5,5,3);    

//     double a = image -> PSNR(image2);
//     EXPECT_DOUBLE_EQ(18.430435842453537,a);
// }

TEST(Array2DTest, BaricentricInterpolation)
{
    uint8_t*** test_img;
    test_img = new uint8_t**[5];
    for(int i=0;i<5;i++)
    {
        test_img[i] = new uint8_t*[5];
        for(int j=0;j<5;j++)
            test_img[i][j] = new uint8_t[1];
    }

    std::vector<std::vector<double>> array =
    {
    {{200}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {100}}
    };
    auto interpolation = new Array2D(array); 
    auto image = std::make_unique<Array2D>(array); 
    interpolation -> FillWith(0);
    interpolation -> BaricentricInterpolation(*image, 0,4,4,0,0,4);
    auto error = interpolation -> SquaredError(*image,0,4,4,0,0,4);
    auto comparison = interpolation -> CompareWith(*image,0,4,4,0,0,4);
    EXPECT_EQ((*interpolation)[2][2],150);

    EXPECT_EQ(error, 143750);
    EXPECT_EQ(comparison, 0.2763360246059208);
}

TEST(Array2DTest, ArrayInitialization)
{
    uint8_t*** test_img;
    test_img = new uint8_t**[5];
    for(int i=0;i<5;i++)
    {
        test_img[i] = new uint8_t*[5];
        for(int j=0;j<5;j++)
            test_img[i][j] = new uint8_t[1];
    }

    std::vector<std::vector<double>> array =
    {
    {{1}, {0}, {0}, {0}, {0}},
    {{0}, {2}, {0}, {0}, {0}},
    {{0}, {0}, {3}, {0}, {0}},
    {{0}, {0}, {0}, {4}, {0}},
    {{0}, {0}, {0}, {0}, {5}}
    };
    auto image = std::make_unique<Array2D>(array); 
    EXPECT_EQ((*image)[0][0], 1);
    EXPECT_EQ((*image)[1][1], 2);
    EXPECT_EQ((*image)[2][2], 3);
    EXPECT_EQ((*image)[3][3], 4);
    EXPECT_EQ((*image)[4][4], 5);
}

TEST(Array2DTest, OffsettedSubtract)
{
    auto image = std::make_unique<Array2D>(5,5); 
    image->FillWith(5);
    auto f = [](double x, double y){return x+y;};
    image->Subtract(f,3,4,3,4,3,3);
    EXPECT_EQ((*image)[4][4], 3);
    EXPECT_EQ((*image)[3][3], 5);
}

TEST(Array2DTest, PointInTriangleTest)
{
    EXPECT_EQ(false, PointInTriangle(0,2,0,0,4,0,4,4));
}