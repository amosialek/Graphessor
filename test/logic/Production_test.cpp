#include "gtest/gtest.h"
#include "Productions.hpp"
#include "mygraph.hpp"
#include "Pixel.hpp"
#include "GraphessorConstants.hpp"

#include <boost/graph/graphviz.hpp>
using namespace std;

TEST(P1Test, StartingProduction)
{
    auto image = new Image("./test_files/face.bmp"); 
    vector<vertex_descriptor> IEdges;
    vector<vertex_descriptor> BEdges;
    MyGraph graph;
    auto S = boost::add_vertex(*(new Pixel(0,0, NODELABEL_S)), graph);
    P1(graph, S, IEdges, BEdges, *image);
}

TEST(P2Test, StartingProduction)
{
    auto image = new Image("./test_files/face.bmp"); 
    MyGraph graph;
    int height = image->height(), width = image->width();
    int r,g,b;
    std::tie(r,g,b) = image->getPixel(0, 0);
    std::cout<<r<<g<<b<<std::endl;
    auto p1 = boost::add_vertex(*(new Pixel(0,0,r,g,b)), graph);
    std::tie(r,g,b) = image->getPixel(width-1, 0);
    auto p2 = boost::add_vertex(*(new Pixel(width-1,0,r,g,b)), graph);
    std::tie(r,g,b) = image->getPixel(0, height-1);
    auto p3 = boost::add_vertex(*(new Pixel(0,height-1,r,g,b)), graph);
    std::tie(r,g,b) = image->getPixel(width-1, height-1);
    auto p4 = boost::add_vertex(*(new Pixel(width-1,height-1,r,g,b)), graph);
    auto I = boost::add_vertex(*(new Pixel(width/2,height/2, NODELABEL_I)), graph);
    graph[I]._break = true;
    graph[I].breakLevel = 3;
    boost::add_edge(p1,I,graph);
    boost::add_edge(p2,I,graph);
    boost::add_edge(p3,I,graph);
    boost::add_edge(p4,I,graph);
    vector<vertex_descriptor> IEdges;
    vector<vertex_descriptor> BEdges;
    vector<vertex_descriptor> FEdges;
    IEdges.push_back(I);
    P2(graph,IEdges, FEdges, *image);
    myEdgeWriter<MyGraph> w(graph);
    //boost::write_graphviz(cerr, graph,w );
    EXPECT_EQ(IEdges.size(),4);
    for(auto IEdge : IEdges)
    {
        EXPECT_EQ(graph[IEdge].breakLevel,4);
    }
}

TEST(P3Test, P3AfterP2AfterP1)
{
    auto image = new Image("./test_files/face.bmp"); 
    vector<vertex_descriptor> IEdges;
    vector<vertex_descriptor> BEdges;
    vector<vertex_descriptor> FEdges;
    MyGraph graph;
    auto S = boost::add_vertex(*(new Pixel(0,0, NODELABEL_S)), graph);
    P1(graph, S, IEdges, BEdges, *image);
    graph[S]._break=1;
    P2(graph,IEdges, FEdges, *image);
    cout<<"BEDGES SIZE: "<<BEdges.size()<<endl;
    P3(graph,BEdges,*image);
    myEdgeWriter<MyGraph> w(graph);
    // boost::write_graphviz(cerr, graph, w);
    EXPECT_EQ(1,1);
}

TEST(P4Test, BasicTest)
{
    auto image = new Image("./test_files/face.bmp"); 
    MyGraph graph;
    std::vector<vertex_descriptor> FEdges;
    int r=255,g=255,b=255;
    int width=10, height=10;
    auto p1 = boost::add_vertex(*(new Pixel(0,4,r,g,b)), graph);
    auto p2 = boost::add_vertex(*(new Pixel(4,0,r,g,b)), graph);
    auto p3 = boost::add_vertex(*(new Pixel(9,4,r,g,b)), graph);
    auto p4 = boost::add_vertex(*(new Pixel(4,9,r,g,b)), graph);
    auto f1 = boost::add_vertex(*(new Pixel(4,6,NODELABEL_F)), graph);
    auto f2 = boost::add_vertex(*(new Pixel(4,2,NODELABEL_F)), graph);
    auto f = boost::add_vertex(*(new Pixel(4,4,NODELABEL_F)), graph);
    auto i1 = boost::add_vertex(*(new Pixel(2,2,NODELABEL_I)), graph);
    auto i2 = boost::add_vertex(*(new Pixel(6,2,NODELABEL_I)), graph);
    auto i3 = boost::add_vertex(*(new Pixel(6,6,NODELABEL_I)), graph);
    auto i4 = boost::add_vertex(*(new Pixel(2,6,NODELABEL_I)), graph);
    add_edge(p1,i1,graph);
    add_edge(p2,i1,graph);
    add_edge(p2,i2,graph);
    add_edge(p3,i2,graph);
    add_edge(p3,i3,graph);
    add_edge(p4,i3,graph);
    add_edge(p4,i4,graph);
    add_edge(p1,i4,graph);
    add_edge(p2,f2,graph);
    add_edge(p4,f1,graph);
    add_edge(p1,f,graph);
    add_edge(p3,f,graph);

    FEdges.push_back(f);
    FEdges.push_back(f1);
    FEdges.push_back(f2);
    
    P4(graph, FEdges, *image);
    myEdgeWriter<MyGraph> w(graph);
    boost::write_graphviz(cerr, graph, w);
    EXPECT_EQ(4, FEdges.size());
}

TEST(P6Test, OneOfTwo)
{
    auto image = new Image("./test_files/face.bmp"); 
    MyGraph graph;
    int r=0,g=0,b=0;
    int width=10;
    auto p1 = boost::add_vertex(*(new Pixel(-width,0,r,g,b)), graph);
    auto p2 = boost::add_vertex(*(new Pixel(width,0,r,g,b)), graph);
    auto IEdge1 = boost::add_vertex(*(new Pixel(-2*width,0,NODELABEL_I)), graph);
    auto IEdge2 = boost::add_vertex(*(new Pixel(2*width, 0,NODELABEL_I)), graph);
    auto IEdge = boost::add_vertex(*(new Pixel(0, 0, NODELABEL_I)), graph);
    graph[IEdge]._break = true;
    graph[IEdge].breakLevel=5;
    graph[IEdge].breakLevel=4;
    graph[IEdge2].breakLevel=5;
    boost::add_edge(p1,IEdge,graph);
    boost::add_edge(p2,IEdge,graph);
    boost::add_edge(p1,IEdge1,graph);
    boost::add_edge(p2,IEdge2,graph);
    vector<vertex_descriptor> IEdges;
    IEdges.push_back(IEdge);
    IEdges.push_back(IEdge1);
    IEdges.push_back(IEdge2);
    P6(graph, IEdges);
    EXPECT_EQ(graph[IEdge1]._break,1);
    EXPECT_EQ(graph[IEdge2]._break,0);
}



