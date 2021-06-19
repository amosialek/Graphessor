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

TEST(GetFEdgeWithTwoVerticesCoefficientsTests, verticalCornerTest)
{
    int orders = 5;
    Array2D image(1,11);
    for(int i=0;i<11;i++)
        image[0][i] = 50 + i * 10; //linear
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0,0, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(0,10, NODELABEL_P)));
    auto [values, wildcard] = GetFEdgeWithTwoVerticesCoefficients(image, graph[P0], graph[P1], 0, 0, 0, 5, orders);

    ASSERT_DOUBLE_EQ(values[0][5],100);
}

TEST(GetFEdgeWithTwoVerticesCoefficientsTests, horizontalCornerTest)
{
    int orders = 5;
    Array2D image(11,1);
    for(int i=0;i<11;i++)
        image[i][0] = 50 + i * 10; //linear
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0,0, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(10,0, NODELABEL_P)));
    auto [values, wildcard] = GetFEdgeWithTwoVerticesCoefficients(image, graph[P0], graph[P1], 0, 5, 0, 0, orders);

    ASSERT_DOUBLE_EQ(values[5][0],100);
}

TEST(GetFEdgeWithTwoVerticesCoefficientsTests, horizontalCornerTestWithParabola)
{
    int orders = 5;
    Array2D image(101,1);
    for(int i=0;i<101;i++)
        image[i][0] = 50 + i * 10 - i/100.0*(i/100.0 - 1) * 7; 
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0,0, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(100,0, NODELABEL_P)));
    auto [values, wildcard] = GetFEdgeWithTwoVerticesCoefficients(image, graph[P0], graph[P1], 0, 50, 0, 0, orders);

    ASSERT_TRUE(abs(values[50][0]-(550+7.0/4.0)<0.01));
}

TEST(GetFEdgeWithTwoVerticesCoefficientsTests, verticalCornerTestWithParabola)
{
    int orders = 5;
    Array2D image(1,101);
    for(int i=0;i<101;i++)
        image[0][i] = 50 + i * 10 - i/100.0*(i/100.0 - 1) * 7; 
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0, 0, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(0, 100, NODELABEL_P)));
    auto [values, wildcard] = GetFEdgeWithTwoVerticesCoefficients(image, graph[P0], graph[P1], 0, 0, 0, 50, orders);

    ASSERT_TRUE(abs(values[0][50]-(550+7.0/4.0)<0.01));
}

TEST(GetFEdgeWithTwoVerticesCoefficientsTests, horizontalCoefficientTestWithParabolaTopHalfEdge)
{
    int orders = 1;
    Array2D image(101,1);
    for(int i=0;i<101;i++)
        image[i][0] = 50 + i * 10 - i/100.0*(i/100.0 - 1) * 7; 
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0,0, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(100,0, NODELABEL_P)));
    auto [wildcard, values] = GetFEdgeWithTwoVerticesCoefficients(image, graph[P0], graph[P1], 0, 50, 0, 100, orders);

    ASSERT_TRUE(abs(values["top"][0]-1.75)<0.1);
}

TEST(GetFEdgeWithTwoVerticesCoefficientsTests, verticalCoefficientTestWithParabolaLeftHalfEdge)
{
    int orders = 1;
    Array2D image(1,101);
    for(int i=0;i<101;i++)
        image[0][i] = 50 + i * 10 - i/100.0*(i/100.0 - 1) * 7; 
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0, 0, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(0, 100, NODELABEL_P)));
    auto [wildcard, values] = GetFEdgeWithTwoVerticesCoefficients(image, graph[P0], graph[P1], 0, 100, 0, 50, orders);

    ASSERT_TRUE(abs(values["left"][0]-1.75)<0.1);
}

TEST(GetFEdgeWithTwoVerticesCoefficientsTests, horizontalCoefficientTestWithParabolaBottomHalfEdge)
{
    int orders = 1;
    Array2D image(101,101);
    for(int i=0;i<101;i++)
        image[i][100] = 50 + i * 10 - i/100.0*(i/100.0 - 1) * 7; 
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0,100, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(100,100, NODELABEL_P)));
    auto [wildcard, values] = GetFEdgeWithTwoVerticesCoefficients(image, graph[P0], graph[P1], 0, 50, 0, 100, orders);

    ASSERT_TRUE(abs(values["bottom"][0]-1.75)<0.1);
}

TEST(GetFEdgeWithTwoVerticesCoefficientsTests, verticalCoefficientTestWithParabolaRightHalfEdge)
{
    int orders = 1;
    Array2D image(101,101);
    for(int i=0;i<101;i++)
        image[100][i] = 50 + i * 10 - i/100.0*(i/100.0 - 1) * 7; 
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(100, 0, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(100, 100, NODELABEL_P)));
    auto [wildcard, values] = GetFEdgeWithTwoVerticesCoefficients(image, graph[P0], graph[P1], 0, 100, 0, 50, orders);

    ASSERT_TRUE(abs(values["right"][0]-1.75)<0.1);
}

TEST(GetFEdgeWithTwoVerticesCoefficientsTests, horizontalCoefficientTestWithParabolaTop)
{
    int orders = 1;
    Array2D image(101,1);
    for(int i=0;i<101;i++)
        image[i][0] = 50 + i * 10 - i/100.0*(i/100.0 - 1) * 7; 
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0,0, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(100,0, NODELABEL_P)));
    auto [wildcard, values] = GetFEdgeWithTwoVerticesCoefficients(image, graph[P0], graph[P1], 0, 100, 0, 100, orders);

    ASSERT_TRUE(abs(values["top"][0]-7)<0.1);
}

TEST(GetFEdgeWithTwoVerticesCoefficientsTests, verticalCoefficientTestWithParabolaLeft)
{
    int orders = 1;
    Array2D image(1,101);
    for(int i=0;i<101;i++)
        image[0][i] = 50 + i * 10 - i/100.0*(i/100.0 - 1) * 7; 
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0, 0, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(0, 100, NODELABEL_P)));
    auto [wildcard, values] = GetFEdgeWithTwoVerticesCoefficients(image, graph[P0], graph[P1], 0, 100, 0, 100, orders);

    ASSERT_TRUE(abs(values["left"][0]-7)<0.1);
}

TEST(GetFEdgeWithTwoVerticesCoefficientsTests, horizontalCoefficientTestWithParabolaBottom)
{
    int orders = 1;
    Array2D image(101,101);
    for(int i=0;i<101;i++)
        image[i][100] = 50 + i * 10 - i/100.0*(i/100.0 - 1) * 7; 
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0,100, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(100,100, NODELABEL_P)));
    auto [wildcard, values] = GetFEdgeWithTwoVerticesCoefficients(image, graph[P0], graph[P1], 0, 100, 0, 100, orders);

    ASSERT_TRUE(abs(values["bottom"][0]-7)<0.1);
}

TEST(GetFEdgeWithTwoVerticesCoefficientsTests, verticalCoefficientTestWithParabolaRight)
{
    int orders = 1;
    Array2D image(101,101);
    for(int i=0;i<101;i++)
        image[100][i] = 50 + i * 10 - i/100.0*(i/100.0 - 1) * 7; 
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(100, 0, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(100, 100, NODELABEL_P)));
    auto [wildcard, values] = GetFEdgeWithTwoVerticesCoefficients(image, graph[P0], graph[P1], 0, 100, 0, 100, orders);

    ASSERT_TRUE(abs(values["right"][0]-7)<0.1);
}

TEST(GetEdgesTests, GetEdgesSunnyDay)
{
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0, 0, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(100, 0, NODELABEL_P)));
    auto P2 = graph.AddVertex(*(make_unique<Pixel>(0, 100, NODELABEL_P)));
    auto P3 = graph.AddVertex(*(make_unique<Pixel>(100, 100, NODELABEL_P)));
    auto F0 = graph.AddVertex(*(make_unique<Pixel>(50, 0, NODELABEL_F)));
    auto F1 = graph.AddVertex(*(make_unique<Pixel>(0, 50, NODELABEL_F)));
    auto F2 = graph.AddVertex(*(make_unique<Pixel>(50, 100, NODELABEL_F)));
    auto F3 = graph.AddVertex(*(make_unique<Pixel>(100, 50, NODELABEL_F)));
    auto I = graph.AddVertex(*(make_unique<Pixel>(50, 50, NODELABEL_I)));
    graph.AddEdge(P0,I);
    graph.AddEdge(P1,I);
    graph.AddEdge(P2,I);
    graph.AddEdge(P3,I);
    
    graph.AddEdge(P0,F0);
    graph.AddEdge(P0,F1);

    graph.AddEdge(P1,F0);
    graph.AddEdge(P1,F3);

    graph.AddEdge(P2,F1);
    graph.AddEdge(P2,F2);

    graph.AddEdge(P3,F2);
    graph.AddEdge(P3,F3);

    auto edges = GetEdges(graph,I,0,100,0,100);
    ASSERT_EQ(4, edges.size());
    ASSERT_EQ(F0,*edges.find(F0));
    ASSERT_EQ(F1,*edges.find(F1));
    ASSERT_EQ(F2,*edges.find(F2));
    ASSERT_EQ(F3,*edges.find(F3));
}

TEST(GetEdgesTests, GetEdgesWithTwoLongEdges)
{
    CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0, 0, NODELABEL_P)));
    auto P3 = graph.AddVertex(*(make_unique<Pixel>(100, 100, NODELABEL_P)));
    auto F0 = graph.AddVertex(*(make_unique<Pixel>(50, 0, NODELABEL_F)));
    auto F1 = graph.AddVertex(*(make_unique<Pixel>(0, 50, NODELABEL_F)));
    auto F2 = graph.AddVertex(*(make_unique<Pixel>(0, 100, NODELABEL_F)));
    auto F3 = graph.AddVertex(*(make_unique<Pixel>(100, 0, NODELABEL_F)));
    auto I = graph.AddVertex(*(make_unique<Pixel>(50, 50, NODELABEL_I)));
    graph.AddEdge(P0,I);
    graph.AddEdge(P3,I);
    
    graph.AddEdge(P0,F0);
    graph.AddEdge(P0,F1);
    graph.AddEdge(P3,F2);
    graph.AddEdge(P3,F3);

    auto edges = GetEdges(graph,I,0,100,0,100);
    ASSERT_EQ(4, edges.size());
    ASSERT_EQ(F0,*edges.find(F0));
    ASSERT_EQ(F1,*edges.find(F1));
    ASSERT_EQ(F2,*edges.find(F2));
    ASSERT_EQ(F3,*edges.find(F3));
}

TEST(GetEdgesTests, GetEdgesWithAdditionalEdges)
{
        CachedGraph graph;
    auto P0 = graph.AddVertex(*(make_unique<Pixel>(0, 0, NODELABEL_P)));
    auto P1 = graph.AddVertex(*(make_unique<Pixel>(100, 0, NODELABEL_P)));
    auto P2 = graph.AddVertex(*(make_unique<Pixel>(0, 100, NODELABEL_P)));
    auto P3 = graph.AddVertex(*(make_unique<Pixel>(100, 100, NODELABEL_P)));
    auto F0 = graph.AddVertex(*(make_unique<Pixel>(50, 0, NODELABEL_F)));
    auto F1 = graph.AddVertex(*(make_unique<Pixel>(0, 50, NODELABEL_F)));
    auto F2 = graph.AddVertex(*(make_unique<Pixel>(50, 100, NODELABEL_F)));
    auto F3 = graph.AddVertex(*(make_unique<Pixel>(100, 50, NODELABEL_F)));

    auto F4 = graph.AddVertex(*(make_unique<Pixel>(-50, 0, NODELABEL_F)));
    auto F5 = graph.AddVertex(*(make_unique<Pixel>(0, -50, NODELABEL_F)));
    auto F6 = graph.AddVertex(*(make_unique<Pixel>(150, 100, NODELABEL_F)));
    auto F7 = graph.AddVertex(*(make_unique<Pixel>(100, 150, NODELABEL_F)));
    auto F8 = graph.AddVertex(*(make_unique<Pixel>(150, 0, NODELABEL_F)));
    auto F9 = graph.AddVertex(*(make_unique<Pixel>(0, 150, NODELABEL_F)));
    auto F10 = graph.AddVertex(*(make_unique<Pixel>(-50, 100, NODELABEL_F)));
    auto F11 = graph.AddVertex(*(make_unique<Pixel>(100, -50, NODELABEL_F)));
    auto I = graph.AddVertex(*(make_unique<Pixel>(50, 50, NODELABEL_I)));
    graph.AddEdge(P0,I);
    graph.AddEdge(P1,I);
    graph.AddEdge(P2,I);
    graph.AddEdge(P3,I);
    
    graph.AddEdge(P0,F0);
    graph.AddEdge(P0,F1);

    graph.AddEdge(P1,F0);
    graph.AddEdge(P1,F3);

    graph.AddEdge(P2,F1);
    graph.AddEdge(P2,F2);

    graph.AddEdge(P3,F2);
    graph.AddEdge(P3,F3);

    //edges which are adjacent but are not edges of IEdge rectangle

    graph.AddEdge(P0,F4);
    graph.AddEdge(P0,F5);
    graph.AddEdge(P3,F6);
    graph.AddEdge(P3,F7);
    graph.AddEdge(P1,F8);
    graph.AddEdge(P1,F11);
    graph.AddEdge(P2,F9);
    graph.AddEdge(P3,F10);

    auto edges = GetEdges(graph,I,0,100,0,100);
    ASSERT_EQ(4, edges.size());
    ASSERT_EQ(F0,*edges.find(F0));
    ASSERT_EQ(F1,*edges.find(F1));
    ASSERT_EQ(F2,*edges.find(F2));
    ASSERT_EQ(F3,*edges.find(F3));
}