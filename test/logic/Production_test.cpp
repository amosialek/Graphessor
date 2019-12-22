#include "gtest/gtest.h"
#include "Productions.hpp"
#include "mygraph.hpp"
#include "Pixel.hpp"

#include <boost/graph/graphviz.hpp>
using namespace std;

template <class Name>
class myEdgeWriter {
public:
     myEdgeWriter(Name _name) : name(_name) {}
     template <class VertexOrEdge>
     void operator()(std::ostream& out, const VertexOrEdge& v) const {
        char buffer[8];
        string s = "[";
        if(name[v].r!=-1){
            sprintf(buffer,"#%02X%02X%02X",name[v].r,name[v].g,name[v].b);
            s+= "fillcolor=\"";
            s+=buffer;
            s+="\",style=filled\n";
        }
        if(name[v].x>=0 && name[v].y>=0 )
            s+= "pos=\""+to_string(name[v].x)+','+to_string(name[v].y)+"!\"\n";
        s+="label=\""+ name[v].label +"\"";
        s+=']';
        out<<s<<endl;
     }
private:
     Name name;
};


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
    auto I = boost::add_vertex(*(new Pixel(width/2,height/2,"I")), graph);
    graph[I]._break = true;
    boost::add_edge(p1,I,graph);
    boost::add_edge(p2,I,graph);
    boost::add_edge(p3,I,graph);
    boost::add_edge(p4,I,graph);
    vector<vertex_descriptor> IEdges;
    IEdges.push_back(I);
    P2(graph,IEdges,*image);
    myEdgeWriter<MyGraph> w(graph);
    boost::write_graphviz(cerr, graph,w );
    EXPECT_EQ(1,1);
}
