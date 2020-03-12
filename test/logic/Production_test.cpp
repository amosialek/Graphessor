#include "gtest/gtest.h"
#include "Productions.hpp"
#include "mygraph.hpp"
#include "Pixel.hpp"
#include "GraphessorConstants.hpp"
#include "CachedGraph.hpp"

#include <boost/graph/graphviz.hpp>
using namespace std;

TEST(P1Test, StartingProduction)
{
    auto image = make_unique<Image>("./test_files/face.bmp"); 
    auto IEdges = make_unique<vector<vertex_descriptor>>();
    auto BEdges = make_unique<vector<vertex_descriptor>>();
    auto graph = make_shared<CachedGraph>();
    auto S = graph -> AddVertex(*(make_unique<Pixel>(0,0, NODELABEL_S)));
    auto p1 = make_unique<P1>(graph, S, move(IEdges), move(BEdges), move(image));
    p1->Perform();
}

TEST(P2Test, StartingProduction)
{
    auto image = make_unique<Image>("./test_files/face.bmp"); 
    auto graph = make_shared<CachedGraph>();
    int height = image->height(), width = image->width();
    int r,g,b;
    std::tie(r,g,b) = image->getPixel(0, 0);
    std::cout<<r<<g<<b<<std::endl;
    auto p1 = graph -> AddVertex(*(make_unique<Pixel>(0,0,r,g,b)));
    std::tie(r,g,b) = image->getPixel(width-1, 0);
    auto p2 = graph -> AddVertex(*(make_unique<Pixel>(width-1,0,r,g,b)));
    std::tie(r,g,b) = image->getPixel(0, height-1);
    auto p3 = graph -> AddVertex(*(make_unique<Pixel>(0,height-1,r,g,b)));
    std::tie(r,g,b) = image->getPixel(width-1, height-1);
    auto p4 = graph -> AddVertex(*(make_unique<Pixel>(width-1,height-1,r,g,b)));
    auto I = graph -> AddVertex(*(make_unique<Pixel>(width/2,height/2, NODELABEL_I)));
    (*graph)[I]._break = true;
    (*graph)[I].breakLevel = 3;
    graph -> AddEdge(p1,I);
    graph -> AddEdge(p2,I);
    graph -> AddEdge(p3,I);
    graph -> AddEdge(p4,I);
    auto IEdges = make_shared<vector<vertex_descriptor>>();
    auto BEdges = make_shared<vector<vertex_descriptor>>();
    auto FEdges = make_shared<vector<vertex_descriptor>>();
    IEdges->push_back(I);
    P2(graph, (*IEdges)[0], FEdges, move(image)).Perform();
    //myEdgeWriter<CachedGraph> w(*graph);
    //boost::write_graphviz(cerr, graph,w );
    EXPECT_EQ(IEdges->size(),4);
    for(auto IEdge : *IEdges)
    {
        EXPECT_EQ((*graph)[IEdge].breakLevel,4);
    }
}

TEST(P3Test, P3AfterP2AfterP1)
{
    auto image = make_shared<Image>("./test_files/face.bmp"); 
    auto IEdges = make_shared<vector<vertex_descriptor>>();
    auto BEdges = make_shared<vector<vertex_descriptor>>();
    auto FEdges = make_shared<vector<vertex_descriptor>>();
    auto graph = make_shared<CachedGraph>();
    auto S = graph -> AddVertex(*(make_unique<Pixel>(0,0, NODELABEL_S)));
    P1(graph, S, IEdges, BEdges, image).Perform();
    (*graph)[S]._break=1;
    P2(graph, (*IEdges)[0], FEdges, image).Perform();
    cout<<"BEDGES SIZE: "<<BEdges->size()<<endl;
    P3(graph,BEdges,move(image)).Perform();
    //myEdgeWriter<CachedGraph> w(*graph);
    // boost::write_graphviz(cerr, graph, w);
    EXPECT_EQ(1,1);
}

TEST(P4Test, BasicTest)
{
    auto image = make_unique<Image>("./test_files/face.bmp"); 
    auto graph = make_shared<CachedGraph>();
    auto FEdges = make_shared<vector<vertex_descriptor>>();
    int r=255,g=255,b=255;
    int width=10, height=10;
    auto p1 = graph -> AddVertex(*(make_unique<Pixel>(0,4,r,g,b)));
    auto p2 = graph -> AddVertex(*(make_unique<Pixel>(4,0,r,g,b)));
    auto p3 = graph -> AddVertex(*(make_unique<Pixel>(9,4,r,g,b)));
    auto p4 = graph -> AddVertex(*(make_unique<Pixel>(4,9,r,g,b)));
    auto f1 = graph -> AddVertex(*(make_unique<Pixel>(4,6,NODELABEL_F)));
    auto f2 = graph -> AddVertex(*(make_unique<Pixel>(4,2,NODELABEL_F)));
    auto f = graph -> AddVertex(*(make_unique<Pixel>(4,4,NODELABEL_F)));
    auto i1 = graph -> AddVertex(*(make_unique<Pixel>(2,2,NODELABEL_I)));
    auto i2 = graph -> AddVertex(*(make_unique<Pixel>(6,2,NODELABEL_I)));
    auto i3 = graph -> AddVertex(*(make_unique<Pixel>(6,6,NODELABEL_I)));
    auto i4 = graph -> AddVertex(*(make_unique<Pixel>(2,6,NODELABEL_I)));
    graph -> AddEdge(p1,i1);
    graph -> AddEdge(p2,i1);
    graph -> AddEdge(p2,i2);
    graph -> AddEdge(p3,i2);
    graph -> AddEdge(p3,i3);
    graph -> AddEdge(p4,i3);
    graph -> AddEdge(p4,i4);
    graph -> AddEdge(p1,i4);
    graph -> AddEdge(p2,f2);
    graph -> AddEdge(p4,f1);
    graph -> AddEdge(p1,f);
    graph -> AddEdge(p3,f);

    FEdges->push_back(f);
    FEdges->push_back(f1);
    FEdges->push_back(f2);
    
    P4(graph, FEdges, move(image)).Perform();
    //myEdgeWriter<CachedGraph> w(*graph);
    //boost::write_graphviz(cerr, *graph, w);
    EXPECT_EQ(4, FEdges->size());
}

TEST(P6Test, OneOfTwo)
{
    auto image = make_unique<Image>("./test_files/face.bmp"); 
    auto graph = make_shared<CachedGraph>();
    int r=0,g=0,b=0;
    int width=10;
    auto p1 = graph -> AddVertex(*(make_unique<Pixel>(-width,0,r,g,b)));
    auto p2 = graph -> AddVertex(*(make_unique<Pixel>(width,0,r,g,b)));
    auto IEdge1 = graph -> AddVertex(*(make_unique<Pixel>(-2*width,0,NODELABEL_I)));
    auto IEdge2 = graph -> AddVertex(*(make_unique<Pixel>(2*width, 0,NODELABEL_I)));
    auto IEdge = graph -> AddVertex(*(make_unique<Pixel>(0, 0, NODELABEL_I)));
    (*graph)[IEdge]._break = true;
    (*graph)[IEdge].breakLevel=5;
    (*graph)[IEdge].breakLevel=4;
    (*graph)[IEdge2].breakLevel=5;
    graph -> AddEdge(p1,IEdge);
    graph -> AddEdge(p2,IEdge);
    graph -> AddEdge(p1,IEdge1);
    graph -> AddEdge(p2,IEdge2);
    auto IEdges = make_shared<vector<vertex_descriptor>>();
    IEdges->push_back(IEdge);
    IEdges->push_back(IEdge1);
    IEdges->push_back(IEdge2);
    P6(graph, IEdges).Perform();
    EXPECT_EQ((*graph)[IEdge1]._break,1);
    EXPECT_EQ((*graph)[IEdge2]._break,0);
}



