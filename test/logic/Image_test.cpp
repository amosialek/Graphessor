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
    a = image->CompareWithInterpolation(8,16,0,4,0);
    EXPECT_DOUBLE_EQ(2500.0/(255*255*6),a);
    a = image->CompareWithInterpolation(0,4,8,16,0);
    EXPECT_DOUBLE_EQ(10000.0/(255*255*6),a);
    a = image->CompareWithInterpolation(8,16,8,16,0);
    EXPECT_DOUBLE_EQ(40000.0/(255*255*9),a);
    
}

TEST(ImageMagnifierComparisonTest, CompareWithInterpolation2)
{
    uint8_t*** test_img;
    test_img = new uint8_t**[5];
    for(int i=0;i<5;i++)
    {
        test_img[i] = new uint8_t*[5];
        for(int j=0;j<5;j++)
            test_img[i][j] = new uint8_t[3];
    }

    uint8_t*** test_img2;
    test_img2 = new uint8_t**[5];
    for(int i=0;i<5;i++)
    {
        test_img2[i] = new uint8_t*[5];
        for(int j=0;j<5;j++)
            test_img2[i][j] = new uint8_t[3];
    }

    uint8_t array [5][5][3] =
    {
    {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {0,0,0}, {100,0,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {0,100,0}, {0,0,0}, {100,0,0}},
    {{0,0,0}, {0,0,0}, {0,0,0}, {100,0,0}, {0,0,0}},
    {{0,0,0}, {100,0,0}, {100,0,0}, {0,0,0}, {100,0,0}}
    };
    for(int i=0;i<5;i++)
        for(int j=0;j<5;j++)
            for(int k=0;k<3;k++)
                test_img[i][j][k]=array[i][j][k];

    uint8_t array2 [5][5][3] =
    {
    {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
    {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}}
    };
    for(int i=0;i<5;i++)
        for(int j=0;j<5;j++)
            for(int k=0;k<5;k++)
                test_img2[i][j][k]=array2[i][j][k];

    auto image = new ImageMagnifier(test_img,5,5,3);    
    auto image2 = new Image(test_img2,5,5,3);    

    double a = image -> PSNR(image2);
    EXPECT_DOUBLE_EQ(18.430435842453537,a);
}

TEST(ImageRestoringTest, BaricentricInterpolation)
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
    {{200}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {100}}
    };
    for(int i=0;i<5;i++)
        for(int j=0;j<5;j++)
            test_img[i][j][0]=array[i][j][0];

    auto image = std::make_unique<Image>(test_img,5,5,1); 
    Pixel p1,p2,p3;
    p1.x=0;
    p1.y=0;
    p1.r=200;
    p2.x=4;
    p2.y=0;
    p2.r=0;
    p3.x=4;
    p3.y=4;
    p3.r=100;
    vector<Pixel> v{p1,p2,p3};

    image -> BaricentricInterpolation(0, v);
    Pixel pixel22;
    std::tie(pixel22.r,pixel22.g, pixel22.b) = image -> getPixel(2,2);
    EXPECT_EQ(pixel22.r,150);

}