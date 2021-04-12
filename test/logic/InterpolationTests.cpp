#include "gtest/gtest.h"
#include<InterpolationHelper.hpp>
#include<Pixel.hpp>
#include<memory.h>
using namespace std;
TEST(GetSingleVertexFEdgeCoefficientsTests, horizontalTestAllZeros)
{
    int orders = 5;
    Array2D image(11,1);
    for(int i=0;i<11;i++)
        image[i][0] = 50 + i * 10; //linear
    CachedGraph graph;
    auto P = graph.AddVertex(*(make_unique<Pixel>(0,0, NODELABEL_P)));
    auto F = graph.AddVertex(*(make_unique<Pixel>(5,0, NODELABEL_F)));
    auto values = GetSingleVertexFEdgeCoefficients(image, graph, F, P,0,0,10,0,150,orders);
    for(int i=0;i<orders;i++)
        ASSERT_DOUBLE_EQ(values[i], 0.0);

    auto P2 = graph.AddVertex(*(make_unique<Pixel>(10,0, NODELABEL_P)));
    auto F2 = graph.AddVertex(*(make_unique<Pixel>(5,0, NODELABEL_F)));
    values = GetSingleVertexFEdgeCoefficients(image, graph, F2, P2,0,0,10,0,50,orders);
    for(int i=0;i<orders;i++)
        ASSERT_DOUBLE_EQ(values[i], 0.0);
}

TEST(GetSingleVertexFEdgeCoefficientsTests, verticalTestAllZeros)
{
    int orders = 5;
    Array2D image(1,11);
    for(int i=0;i<11;i++)
        image[0][i] = 50 + i * 10; //linear
    CachedGraph graph;
    auto P = graph.AddVertex(*(make_unique<Pixel>(0,0, NODELABEL_P)));
    auto F = graph.AddVertex(*(make_unique<Pixel>(0,5, NODELABEL_F)));
    auto values = GetSingleVertexFEdgeCoefficients(image, graph, F, P,0,0,0,10,150,orders);
    for(int i=0;i<orders;i++)
        ASSERT_DOUBLE_EQ(values[i], 0.0);

    auto P2 = graph.AddVertex(*(make_unique<Pixel>(0,10, NODELABEL_P)));
    auto F2 = graph.AddVertex(*(make_unique<Pixel>(0,5, NODELABEL_F)));
    values = GetSingleVertexFEdgeCoefficients(image, graph, F2, P2,0,0,0,10,50,orders);
    for(int i=0;i<orders;i++)
        ASSERT_DOUBLE_EQ(values[i], 0.0);
}

TEST(GetSingleVertexFEdgeCoefficientsTests, horizontalTestWithParabola)
{
    int orders = 1;
    Array2D image(101,1);
    for(int i=0;i<101;i++)
        image[i][0] = 50 + i * 10 - i/100.0*(i/100.0 - 1) * 7; 
    CachedGraph graph;
    auto P = graph.AddVertex(*(make_unique<Pixel>(0,0, NODELABEL_P)));
    auto F = graph.AddVertex(*(make_unique<Pixel>(5,0, NODELABEL_F)));
    auto values = GetSingleVertexFEdgeCoefficients(image, graph, F, P,0,0,100,0,1050,orders);
 
    auto P2 = graph.AddVertex(*(make_unique<Pixel>(100,0, NODELABEL_P)));
    auto F2 = graph.AddVertex(*(make_unique<Pixel>(5,0, NODELABEL_F)));
    values = GetSingleVertexFEdgeCoefficients(image, graph, F2, P2,0,0,100,0,50,orders);
    ASSERT_TRUE(abs(values[0]-7)<0.1);
}

TEST(GetSingleVertexFEdgeCoefficientsTests, verticalTestWithParabola)
{
    int orders = 1;
    Array2D image(1, 101);
    for(int i=0;i<101;i++)
        image[0][i] = 50 + i * 10 - i/100.0*(i/100.0 - 1) * 7; 
    CachedGraph graph;
    auto P = graph.AddVertex(*(make_unique<Pixel>(0,0, NODELABEL_P)));
    auto F = graph.AddVertex(*(make_unique<Pixel>(0,5, NODELABEL_F)));
    auto values = GetSingleVertexFEdgeCoefficients(image, graph, F, P,0,0,0,100,1050,orders);
    
    auto P2 = graph.AddVertex(*(make_unique<Pixel>(0,100, NODELABEL_P)));
    auto F2 = graph.AddVertex(*(make_unique<Pixel>(0,5, NODELABEL_F)));
    values = GetSingleVertexFEdgeCoefficients(image, graph, F2, P2,0,0,0,100,50,orders);
    ASSERT_TRUE(abs(values[0]-7)<0.1);
}